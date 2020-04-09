#include "quadtree.hpp"
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

