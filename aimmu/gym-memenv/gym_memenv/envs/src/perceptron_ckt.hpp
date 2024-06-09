/*
This module is to tune the hyper parameters using simple perceptron based 
neural circuit trained on feedback based mechanism in the hardware.
*/

#ifndef _PERCEPTRON_CKT_HPP_
#define _PERCEPTRON_CKT_HPP_

#include "config_utils.hpp"
#include "extern_vars.hpp"
#include <vector>
/*
 * Simple form of neuron is called perceptron, implemented using inputs and weights
 * Ref. Dynamic Branch Prediction with Perceptrons, Jimenez et. al
 */
class Perceptron{
  public: 
    Perceptron(const Configuration &config);
    ~Perceptron();
    
    double calc_y();
    void update_weights(double);
    void enter_new_input(double);

  private:
    int _id;
    double _latest_y;
    double _theta;
    double _max_param;
    int _max_input;
    int _cirular_fifo_index;
    /*
     * size of the input is fixed using the config, implemented as a FIFO
     */
    deque<double>_input;//shifts with each page access
    vector<double>_weights;//number of wights are fixed to the number of inputs + 1
};

/*
 * Instead of table of perceptrons (basically perceptron network), we will have a table 
 * of neural circuit. A neural circuit is a population of neurons interconnected by 
 * synapses to carry out a specific task when activated. 
 * Ref. Purves, Dale (2011). Neuroscience (5th ed.). Sunderland, Mass.: 
 * Sinauer. p. 507. ISBN 9780878936953. 
 *
 * In our context we want to have separate perceptrons for hop, pg miss rate, agent's 
 * training cycle, epoch reward then their output will be weighted again to come up 
 * with a decision. These four perceptron/neurons will form a neural circuit. We will 
 * have finite number of neural circuits that will be selected depending on the partial 
 * page address. There are five neurons in the following figure. 
 * 
 *     \
 *    --(0)---\
 *     /       \
                \
 *     \         \ 
 *    --(1)------ \
 *     /           \
 *                  (4)---
 *     \           /
 *    --(2)-------/
 *     /         /
                / 
 *     \       /
 *    --(3)---/
 *     /
 * 
 *   |<--L0-->|<-- L1-->|    
 */

class NeuralCircuit{
  public:
    NeuralCircuit(const Configuration &config);//we configure (#inputs and #weights) the circuit members here
    ~NeuralCircuit();
        
    double inference();//returns the action
    void training(vector<double>&);//weight update rule/training
    void take_input(vector<double>&);//take input 
  private:
    int _perceptron_size;
    int _neural_circuit_size;
    vector<Perceptron>_CircuitMembers;
};

/*
 * We want to decide adaptively when short epoch and when long epoch is necessary !!
 * 
 * Intuition: 
 *  1. When hop count across epoch is steady and low, not many migrations needed, 
 *     longer epoch may seem to be helpful to keep the system performance stable.
 *     1.1 longer epoch may result into unstability because of explorations, assuming
 *         learning is 100% accurate
 *  2. When the hop count across epoch is steady high, or fluctuating a lot,
 *     shorter epoch could be helpful to facilitate large number of migrations.
 *  
 *  Possible parameters for feedback/train the perceptrons
 *    --> hop count or, hop count difference with the last epoch
 *    --> page misses between two epochs -| these two together hold the idea of a phase
 *    --> number of operations issued    -| 
 *    --> agent's training cycle and corresponsding reward 
 * 
 *  Since we are using virtual address to select a perceptron, depending on the bits
 *  we are choosing, the perceptrons will be trained differently. What should be the 
 *  logistic behind chossing the bits in the virtual address for deciding the index? 
 */

class AdaptiveEpoch{
  public:
    AdaptiveEpoch(const Configuration &config);
    ~AdaptiveEpoch();
    /*
     * predicts whether the epoch should end at that instant
     * --> vaddr, used for calculating the index for the perceptron table
     * --> input, is supplied with each page access --> should make change in the input queue
     */
    bool predict_epoch_end(unsigned long vaddr);
    void get_input(unsigned long vaddr, vector<double>&);
    void training_epoch(unsigned long vaddr, vector<double>&);
 
  private:
    double _threshold;
    int _num_neural_ckt; 
    /*
     * index for the PerceptronTab is calculated using page address
     */
    vector<NeuralCircuit>_NeuralCircuitNetwork;//holds all the perceptron
};

#endif
