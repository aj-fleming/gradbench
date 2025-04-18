#include "gradbench/evals/lse.hpp"
#include "gradbench/main.hpp"
#include <algorithm>
#include <vector>

#include <adolc/adouble.h>
#include <adolc/drivers/drivers.h>
#include <adolc/taping.h>

static const int tapeTag = 1;

class Gradient : public Function<lse::Input, lse::GradientOutput> {
public:
  Gradient(lse::Input& input) : Function(input) {
    size_t n = _input.x.size();

    trace_on(tapeTag);

    std::vector<adouble> x_d(n);
    for (size_t i = 0; i < n; i++) {
      x_d[i] <<= _input.x[i];
    }

    adouble primal_out_d;
    lse::primal(n, x_d.data(), &primal_out_d);

    double primal_out;
    primal_out_d >>= primal_out;

    trace_off();
  }

  void compute(lse::GradientOutput& output) {
    size_t n = _input.x.size();
    output.resize(n);

    gradient(tapeTag, n, _input.x.data(), output.data());
  }
};

int main(int argc, char* argv[]) {
  return generic_main(argc, argv,
                      {{"primal", function_main<lse::Primal>},
                       {"gradient", function_main<Gradient>}});
}
