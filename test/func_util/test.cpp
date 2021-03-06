#include <iostream>
#include "../../cranberries/func_util.hpp"
#include "../../cranberries/utility.hpp"
struct Twice {
  template < class T >
  constexpr auto operator()(T a_) const noexcept { return a_ * 2; }
};
struct Add {
  template < class T >
  constexpr auto operator()(T a_, T b_) const noexcept { return a_ + b_; }
};
struct Sum 
{
    template < class Head, class... Tail >
    constexpr auto operator()(Head&& head, Tail&&... tail) const{
        auto result = head;
        (void)std::initializer_list<int>{ (void( result += tail),0)... };
        return result;
    }
};


constexpr int test(int,int,int) { return 5; }
constexpr int add2(int a) { return a+2; }
constexpr int twice(int a) { return a*2; }
constexpr int minus2(int a) { return a-2; }

int main() {
  using cranberries::composition;

  auto println = [](auto&& head, auto&&... tail) {
    std::cout << head;
    (void)cranberries::SwallowsNest{ (void(std::cout << " " << tail),0)... };
    std::cout << std::endl;
  };

  constexpr auto composite = composition(add2, twice, minus2);
  static_assert( composite(3) == 4, "");

    
  constexpr auto curried_sum = cranberries::curried<Sum>{};
  
  static_assert(curried_sum(1)(2)(3)(4)(5)() == 15, "");
  static_assert(curried_sum(1, 2, 3)(4)(5)() == 15, "");

  constexpr int value = cranberries::curry_<4>(Sum{})(1)(2)(3)(4);
  static_assert(value==10, "");
  static_assert(cranberries::curry_(test)(0)(0)(0)==5, "");
  using namespace cranberries::func_util;

  std::cout << "all apply" << "\n";
  println <<= std::forward_as_tuple(1, 2, 3, 4);


  std::cout << "each apply" << "\n";
  println |= std::forward_as_tuple(1, 2, 3, 4);

  std::cout << "chunk apply" << "\n";
  println |= chunk<2>::bind(1, 2, 3, 4, 5, 6);

  constexpr Twice f{};
  constexpr Add g{};


  std::cout << "conbinator & chunk apply" << "\n";
  constexpr auto var1 = composition(f, g) |= chunk<2>::bind(1, 2, 3, 4, 5, 6);
  static_assert(
    std::get<0>(var1) == 6 &&
    std::get<1>(var1) == 14 &&
    std::get<2>(var1) == 22,
    ""
    );

  println <<= composition(f, g) |= chunk<2>::bind(1, 2, 3, 4, 5, 6);



  std::cout << "conbinator & adjacent apply" << "\n";
  constexpr auto var2 = composition(f, g) |= adjacent<2>::bind(1, 2, 3, 4, 5, 6);
  static_assert(
    std::get<0>(var2) == 6 &&
    std::get<1>(var2) == 10 &&
    std::get<2>(var2) == 14,
    ""
    );
  println <<= composition(f, g) |= adjacent<2>::bind(1, 2, 3, 4);

  std::cout << "conbinator & chunk apply(random sampling)" << "\n";
  println <<= composition(f, g) |= chunk<2, randomized<10>>::bind(1, 2, 3, 4, 5, 6);

  std::cout << "permutations" << "\n";
  println |= permutation<2>::bind(1, 2, 3);

  std::cout << "combinations" << "\n";
  println |= combination<2>::bind(1, 2, 3);
}