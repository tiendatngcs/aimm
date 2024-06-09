#include "perceptron_ckt.hpp"
#include "../../configs/config_utils.hpp"

int main(int argc, char **argv){
  Configuration config;
  AdaptiveEpoch *adapt_epoch = new AdaptiveEpoch(config);
  return 0;
}
