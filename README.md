# Proxy: Easy Polymorphism in C++

[![Proxy-CI](https://github.com/microsoft/proxy/actions/workflows/pipeline-ci.yml/badge.svg)](https://github.com/microsoft/proxy/actions/workflows/pipeline-ci.yml)

Do you want to save your effort in lifetime management and architecture maintenance of polymorphic objects in C++?

Do you want to be able to write polymorphic code in C++ as easily as in those languages with GC (like Java or C#), while still having excellent runtime performance?

Have you tried other polymorphic programming libraries in C++ but found them somewhat deficient?

If so, this library is for you. 😉

The "proxy" is a C++ library that Microsoft uses to make runtime polymorphism easier to implement, both in architecture design and runtime performance. Please find the design details at https://wg21.link/p0957.

## Quick start

This is a header-only C++20 library. Once you set the language level of your compiler not earlier than C++20 and get the header file ([proxy.h](proxy.h)), you are all set. All the facilities of the library are defined in namespace `pro`. The 3 major class templates are `dispatch`, `facade` and `proxy`. Here is a demo showing how to use this library to implement runtime polymorphism in a different way from the traditional inheritance-based approach:

```cpp
// Abstraction
struct Draw : pro::dispatch<void(std::ostream&)> {
  template <class T>
  void operator()(const T& self, std::ostream& out) { self.Draw(out); }
};
struct Area : pro::dispatch<double()> {
  template <class T>
  double operator()(const T& self) { return self.Area(); }
};
struct DrawableFacade : pro::facade<Draw, Area> {};

// Implementation
class Rectangle {
 public:
  void Draw(std::ostream& out) const
      { out << "{Rectangle: width = " << width_ << ", height = " << height_ << "}"; }
  void SetWidth(double width) { width_ = width; }
  void SetHeight(double height) { height_ = height; }
  double Area() const { return width_ * height_; }

 private:
  double width_;
  double height_;
};

// Client - Consumer
std::string PrintDrawableToString(pro::proxy<DrawableFacade> p) {
  std::stringstream result;
  result << std::fixed << std::setprecision(5) << "shape = ";
  p.invoke<Draw>(result);
  result << ", area = " << p.invoke<Area>();
  return std::move(result).str();
}

// Client - Producer
pro::proxy<DrawableFacade> CreateRectangleAsDrawable(int width, int height) {
  Rectangle rect;
  rect.SetWidth(width);
  rect.SetHeight(height);
  return pro::make_proxy<DrawableFacade>(rect);
}
```

Please find more details and discussions in the spec. The complete version of the "drawable" demo could be found in [tests/proxy_creation_tests.cpp](tests/proxy_creation_tests.cpp).

## Minimum requirements for compilers

| Family | Minimum version | Required flags |
| --- | --- | --- |
| clang | 13.0.0 | -std=c++20 |
| gcc | 11.2 | -std=c++20 |
| MSVC | 19.30 | /std:c++20 |

## Build with CMake

```
git clone https://github.com/microsoft/proxy.git
cd proxy
cmake -S . -B build
cmake --build ./build -j8
cd ./build
ctest -j8
```

## Known issues

We did not notice a bug when testing with gcc or MSVC, but clang will fail to compile if the `minimum_destructibility` is set to `constraint_level::trivial` in a facade definition. The root cause of this failure is that the implementation requires the language feature defined in [P0848R3: Conditionally Trivial Special Member Functions](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p0848r3.html), but it has not been implemented in clang, according to its [documentation](https://clang.llvm.org/cxx_status.html) for the time being.

## Contributing

This project welcomes contributions and suggestions.  Most contributions require you to agree to a
Contributor License Agreement (CLA) declaring that you have the right to, and actually do, grant us
the rights to use your contribution. For details, visit https://cla.opensource.microsoft.com.

When you submit a pull request, a CLA bot will automatically determine whether you need to provide
a CLA and decorate the PR appropriately (e.g., status check, comment). Simply follow the instructions
provided by the bot. You will only need to do this once across all repos using our CLA.

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/).
For more information see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or
contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.

## Trademarks

This project may contain trademarks or logos for projects, products, or services. Authorized use of Microsoft 
trademarks or logos is subject to and must follow 
[Microsoft's Trademark & Brand Guidelines](https://www.microsoft.com/en-us/legal/intellectualproperty/trademarks/usage/general).
Use of Microsoft trademarks or logos in modified versions of this project must not cause confusion or imply Microsoft sponsorship.
Any use of third-party trademarks or logos are subject to those third-party's policies.
