#include "perceptron_ckt.hpp"

/*
 * configure each perceptron/class of perceptrons
 */
Perceptron::Perceptron(const Configuration &config){
  _max_input = config.GetInt("perceptron_size");//maximum number of inputs
  _theta = config.GetFloat("theta");
  _max_param = _max_input;
  _cirular_fifo_index = 0;
  _id = perceptron_id++;
  _latest_y = 0;

  if(_weights.size()==0){
    _weights.resize(_max_input+1);
    _input.resize(_max_input);
    fill(_weights.begin(), _weights.end(), 1);
    fill(_input.begin(), _input.end(), 0.00001);
  }
}

Perceptron::~Perceptron(){
}

double Perceptron::calc_y(){
  double y = 1 * _weights[0];
  for(int i=0; i< _input.size(); i++){
    y += _input[i]*_weights[i+1]; //sum = w0*1 + SUM(xi * wi)
    //cout<<"[perceptron] id: "<<_id<<" input: "<<_input[i]<<" weight: "<<_weights[i+1]<<endl;
  }
  _latest_y = y;
  return y;  
}

/*
 * The param value is always the average in that epoch at that moment.
 * Concern: since the input is always keep changing when, how and why the
 * the weights will be settled?
 */
void Perceptron::update_weights(double param){//collected from system
  if(param!=0){
    assert(_input.size() <= _weights.size());
    //assert(param < _max_param);
    if(_latest_y > param){//lower is better 
      for(int i=0; i<=_input.size(); i++){
        _weights[i] += (param/_max_param); //* _input[i];      
      }
    }
    else{
      for(int i=0; i<=_input.size(); i++){
        _weights[i] -= (param/_max_param); //* _input[i];      
      }
    }
  }
}

/*
 * It is going to be sliding window
 */
void Perceptron::enter_new_input(double inp){
  _input[_cirular_fifo_index] = inp;
  if(_cirular_fifo_index>=_max_input){
    _cirular_fifo_index = 0;
  }
  else{
    _cirular_fifo_index++;
  }
}

/*
 * Building the neural circuit using multiple neurons
 * currently it is fixed two layer circuit which can be
 * easily extendable to any number of layers
 */
NeuralCircuit::NeuralCircuit(const Configuration &config){
  _perceptron_size = config.GetInt("perceptron_size");
  _neural_circuit_size = config.GetInt("neural_circuit_size");
  /*
   * build the neural circuit
   * the connection among them simple, and hence implecit
   * TODO need to have explicit build network for generalization
   */
  for(int p=0; p < _neural_circuit_size; p++){
    Perceptron PerCep(config);
    _CircuitMembers.push_back(PerCep); 
  }
}

NeuralCircuit::~NeuralCircuit(){
}

/*
 * This logic calculates the output of the Neural Circuit
 */
double NeuralCircuit::inference(){
  vector<double>temp_y;
  //layer-0 output calculation
  for(int i=0; i<_CircuitMembers.size()-1; i++){
    temp_y.push_back(_CircuitMembers[i].calc_y());
  }
  assert(temp_y.size()>0);
  //giving input from layer-0 to layer-1
  for(int i=0; i< temp_y.size(); i++){
    _CircuitMembers[4].enter_new_input(temp_y[i]);
  }
  //layer-1 output calculation
  return _CircuitMembers[4].calc_y();//this value will be comapred with Theta
}

/*
 * This is the training function for the Neural Circuit
 * Arguments:
 *  --> param, collected from system for each of the perceptrons in layer-0
 *  --> decision was right or wrong, will be used for update in layer-1
 */

void NeuralCircuit::training(vector<double> &params){
  for(int i=0; i<_CircuitMembers.size(); i++){ 
    _CircuitMembers[i].update_weights(params[i]); 
  }
}

void NeuralCircuit::take_input(vector<double>&inp){
  for(int i=0; i<_CircuitMembers.size(); i++){
    for(int j=0; j<inp.size(); j++){ 
      assert(inp[j] >= 0);
      _CircuitMembers[i].enter_new_input(inp[j]);
    } 
  }
}

AdaptiveEpoch::AdaptiveEpoch(const Configuration &config){
  _threshold = config.GetFloat("neural_cache_threshold");//neural cache threshold
  _num_neural_ckt = config.GetInt("num_neural_ckt");//number of neural ckts
  //cout<<" _threshold: "<<_threshold<<" _num_neural_ckt: "<<_num_neural_ckt<<endl; 
  for(int i=0; i<_num_neural_ckt; i++){
    NeuralCircuit nc(config);
    _NeuralCircuitNetwork.push_back(nc); 
  }
}

AdaptiveEpoch::~AdaptiveEpoch(){
}

bool AdaptiveEpoch::predict_epoch_end(unsigned long vaddr){
   unsigned long pg = vaddr >> 4;
   int mask = _num_neural_ckt - 1;//if num==32 we will get (11111)_2 --> 31
   int index = pg & mask;
   //cout<<"[AdaptiveEpoch] "<<"NC-id: "<<index<<" inference: "<<_NeuralCircuitNetwork[index].inference()<<" threshold: "<<_threshold<<endl;
   return (_NeuralCircuitNetwork[index].inference() <= _threshold)? true : false; 
}

void AdaptiveEpoch::training_epoch(unsigned long vaddr, vector<double> &params){
   unsigned long pg = vaddr >> 4;
   int mask = _num_neural_ckt - 1;//if num==32 we will get (11111)_2 --> 31
   int index = pg & mask;
   _NeuralCircuitNetwork[index].training(params); 
}

void AdaptiveEpoch::get_input(unsigned long vaddr, vector<double> &inp){
   unsigned long pg = vaddr >> 4;
   int mask = _num_neural_ckt - 1;//if num==32 we will get (11111)_2 --> 31
   int index = pg & mask;
   _NeuralCircuitNetwork[index].take_input(inp); 
}
