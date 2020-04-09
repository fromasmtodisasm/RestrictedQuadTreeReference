#include "quadtree.hpp"
class CRender : public IRender {
  void draw_plane(double ox, double oy, double size, color3 color) override {
    using namespace std;
    cout << "\"ox\": " << ox << ","
         << "\"oy\": " << oy << ","
         << "\"size\": " << size;
  }
};

/*
void draw_recursive(QuadTreeRef qt, IRender *render, double ox, double oy,
                    int level, bool is_last);

void draw(QuadTreeRef qt, IRender *render, double ox, double oy, int level) {
  using namespace std;
  cout.precision(8);
  cout << string(" ", level) << "{" << endl;
  cout << string(" ", level + 1) << "\"root\": " << endl;
  draw_recursive(qt, render, ox, oy, level + 1, true);
  cout << string(" ", level) << "}" << endl;
}

void draw_recursive(QuadTreeRef qt, IRender *render, double ox, double oy,
                    int level, bool is_last) {
  using namespace std;

  if (qt->m_children.size() == 0) {
  } else {
    cout << string(level, ' ') << "[" << endl;
    for (int i = 0; i < 4; i++) {
      auto offset = qt->get_offset_by_index(i);
      auto origin = qt->get_origin(qt.get(), i);
      draw_recursive(qt->m_children[i], render, std::get<0>(origin),
                     std::get<0>(origin), level + 1, i == 3);
    }
    if (!is_last)
      cout << string(level, ' ') << "]," << endl;
    else
      cout << string(level, ' ') << "]" << endl;
  }
}
*/

using namespace std;

class TreeRender : public ITreeVisitorCallback {
public:
  TreeRender(IRender *render) : render(render) {}
  // Inherited via ITreeVisitorCallback
  virtual void BeforVisit(QuadTree *qt) override {
    using namespace std;
    cout.precision(8);
    cout << string(" ", 0) << "{" << endl;
    cout << string(" ", 1) << "\"root\": " << endl;
  }
  virtual void AfterVisit(QuadTree *qt) override {
    cout << string(" ", 0) << "}" << endl;
  }
  virtual void OnLeaf(QuadTree *qt, bool is_last, int level) override {
    cout << string(level, ' ') << "{ ";
    render->draw_plane(qt->m_x, qt->m_y, qt->m_size, qt->m_color);
    if (!is_last)
      cout << " },";
    else
      cout << " }";
    cout << endl;
  }
  virtual void BeforeRecursioCall(QuadTree *qt, bool is_last,
                                  int level) override {
    cout << string(level, ' ') << "[" << endl;
  }
  virtual void AfterRecursioCall(QuadTree *qt, bool is_last,
                                 int level) override {
    if (!is_last)
      cout << string(level, ' ') << "]," << endl;
    else
      cout << string(level, ' ') << "]" << endl;
  }

  IRender *render = nullptr;
};

int main() {
  auto quadTree = std::make_shared<QuadTree>(8, 2, 0, 0, color3(1, 0, 0));
  std::shared_ptr<IRender> render = std::make_shared<CRender>();
  TreeRender treeRender(render.get());
  quadTree->split(0.5, 0.5, 1.5f);
  quadTree->visit(&treeRender, 0, 0, 0);
  // draw(quadTree, render.get(), 0, 0, 0);
  return 0;
}
