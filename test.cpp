
#include <algorithm>
#include <array>
#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

struct color3 {
  double r, g, b;
  color3(double r, double g, double b) : r(r), g(g), b(b) {}
  color3() = default;
};

struct IRender {
  virtual void draw_plane(double ox, double oy, double size, color3 color) = 0;
};

static color3 ltc = color3(1, 0, 0);
static color3 rtc = color3(0, 0, 1);
static color3 lbc = color3(0, 1, 0);
static color3 rbc = color3(0, 1, 1);

class QuadTree {
  struct Quad {
    double ox, oy;
    color3 color;
    Quad(double ox, double oy, color3 color) : ox(ox), oy(oy), color(color) {}
    Quad() = default;
  };

public:
  using Ptr = std::shared_ptr<QuadTree>;
  QuadTree(int depth, double size, double x, double y, color3 color)
      : m_depth(depth), m_size(size), m_x(x), m_y(y), m_color(color) {}
  auto get_offset_by_index(int i) {
    std::tuple<int, int> offset;
    switch (i) {
    case 0:
      offset = std::make_tuple<int, int>(-1, -1);
      break;
    case 1:
      offset = std::make_tuple<int, int>(-1, 1);
      break;
    case 2:
      offset = std::make_tuple<int, int>(1, -1);
      break;
    case 3:
      offset = std::make_tuple<int, int>(1, 1);
      break;
    default:
      offset = std::make_tuple<int, int>(0, 0);
      break;
    }
    return offset;
  }

  auto get_node_size(double parent_size) { return 0.5 * parent_size; }

  auto get_origin(QuadTree *parent, int n) {
    auto offset = get_offset_by_index(n);
    return std::make_tuple<double, double>(
        parent->m_x + 0.5 * get_node_size(parent->m_size) * std::get<0>(offset),
        parent->m_y +
            0.5 * get_node_size(parent->m_size) * std::get<1>(offset));
  }

  bool need_split(double x, double y, double ox, double oy, double L,
                  double k) {
    if (m_depth > 0) {
      auto d = std::max(std::min(std::abs(x - ox), std::abs(x - ox - L)),
                        std::min(std::abs(y - oy), std::abs(y - oy - L)));
      return d < k * L;
    }
    return false;
  }

  Quad get_quad(int i) {
    color3 color;
    auto origin = get_origin(this, i);
    switch (i) {
    case 0:
      color = lbc;
      break;
    case 1:
      color = ltc;
      break;
    case 2:
      color = rtc;
      break;
    case 3:
      color = rbc;
      break;
    }
    return Quad(std::get<0>(origin), std::get<1>(origin), color);
  }

  QuadTree::Ptr make_child(int i) {
    auto quad = get_quad(i);
    return std::make_shared<QuadTree>(m_depth - 1, 0.5 * m_size, quad.ox,
                                      quad.oy, quad.color);
  }

  void split(double px, double py, double k) {
    if (need_split(px, py, m_x - 0.5 * m_size, m_y - 0.5 * m_size, m_size, k)) {
      m_children.resize(4);
      for (int i = 0; i < 4; i++) {
        m_children[i] = make_child(i);
        m_children[i]->split(px, py, k);
      }
    }
  }

  void draw(IRender *render, double ox, double oy, int level) {
    using namespace std;
    cout.precision(8);
    cout << string(" ", level) << "{" << endl;
    cout << string(" ", level + 1) << "\"root\": " << endl;
    draw_recursive(render, ox, oy, level + 1, true);
    cout << string(" ", level) << "}" << endl;
  }

  void draw_recursive(IRender *render, double ox, double oy, int level,
                      bool is_last) {
    using namespace std;

    if (m_children.size() == 0) {
      cout << string(level, ' ') << "{ ";
      render->draw_plane(m_x, m_y, m_size, m_color);
      if (!is_last)
        cout << " },";
      else
        cout << " }";
      cout << endl;
    } else {
      cout << string(level, ' ') << "[" << endl;
      for (int i = 0; i < 4; i++) {
        auto offset = get_offset_by_index(i);
        auto origin = get_origin(this, i);
        m_children[i]->draw_recursive(render, std::get<0>(origin),
                                      std::get<0>(origin), level + 1, i == 3);
      }
      if (!is_last)
        cout << string(level, ' ') << "]," << endl;
      else
        cout << string(level, ' ') << "]" << endl;
    }
  }

private:
  int m_depth;
  double m_size;
  double m_x, m_y;
  color3 m_color;
  std::vector<std::shared_ptr<QuadTree>> m_children;
};

class CRender : public IRender {
  void draw_plane(double ox, double oy, double size, color3 color) override {
    using namespace std;
    cout << "\"ox\": " << ox << ","
         << "\"oy\": " << oy << ","
         << "\"size\": " << size;
  }
};

int main() {
  auto quadTree = std::make_shared<QuadTree>(8, 2, 0, 0, color3(1, 0, 0));
  std::shared_ptr<IRender> render = std::make_shared<CRender>();
  quadTree->split(0.5, 0.5, 1.5f);
  quadTree->draw(render.get(), 0, 0, 0);

  return 0;
}
