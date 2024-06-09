import numpy as np
import gym
import gym_memenv

from keras import layers
from keras.models import Sequential
from keras.layers import Dense, Activation, Flatten, Bidirectional, LSTM, Embedding, Dropout, Conv1D
from keras.optimizers import Adam

from aimmu_agent import AIMMUAgent

from rl.policy import EpsGreedyQPolicy
#from rl.agents.dqn import DQNAgent
#from rl.policy import BoltzmannQPolicy
#from rl.policy import GreedyQPolicy
#from rl.policy import LinearAnnealedPolicy
#from rl.policy import BoltzmannGumbelQPolicy

from rl.memory import SequentialMemory
from temp_memory import TempSequentialMemory  


ENV_NAME = 'memenv-v0'


# Get the environment and extract the number of actions.
env = gym.make(ENV_NAME)
np.random.seed(123)
env.seed(123)
nb_actions = env.action_space.n

# Next, we build a very simple model regardless of the dueling architecture
# if you enable dueling network in DQN , DQN will build a dueling network base on your model automatically
# Also, you can build a dueling network by yourself and turn off the dueling network in DQN.
#model = Sequential()
#model.add(Flatten(input_shape=(1,) + env.observation_space.shape))
#model.add(Dense(256))
#model.add(Activation('relu'))
#model.add(Dense(1024))
#model.add(Activation('relu'))
#model.add(Dense(1024))
#model.add(Activation('relu'))
#model.add(Dense(1024))
#model.add(Activation('relu'))
#model.add(Dense(1024))
#model.add(Activation('relu'))
#model.add(Dense(1024))
#model.add(Activation('relu'))
#model.add(Dense(256))
#model.add(Activation('relu'))
#model.add(Dense(256))
#model.add(Activation('relu'))
#model.add(Dense(256))
#model.add(Activation('relu'))
#model.add(Dense(256))
#model.add(Activation('relu'))
#model.add(Dense(256))
#model.add(Activation('relu'))
#model.add(Dense(256))
#model.add(Activation('relu'))
#model.add(Dense(nb_actions, activation='linear'))
##################################################

model = Sequential()
model.add(Flatten(input_shape=(1,) + env.observation_space.shape))
model.add(Dense(16))
model.add(Activation('relu'))
model.add(Dense(16))
model.add(Activation('relu'))
model.add(Dense(16))
model.add(Activation('relu'))
model.add(Dense(16))
model.add(Activation('relu'))
model.add(Dense(nb_actions, activation='linear'))

#######################################################
#model = Sequential()
##model.add(Bidirectional(LSTM(16, return_sequences=True), input_shape=(1, 257)))
##model.add(Embedding((2, 20), input_shape=(1, 2)))
#model.add(Bidirectional(LSTM(10, return_sequences=True), input_shape=(1, 16)))
#model.add(Bidirectional(LSTM(10)))
#model.add(Dropout(0.2))
#model.add(Dense(16))
#model.add(Activation('relu'))
#model.add(Dense(64))
#model.add(Activation('relu'))
#model.add(Dense(128))
#model.add(Activation('relu'))
#model.add(Dense(16))
#model.add(Activation('relu'))
#model.add(Dense(nb_actions, activation='linear'))

#######################################################
#model = Sequential()
#model.add(layers.Conv1D(32, 1, activation='relu',input_shape=(1,16)))
#model.add(layers.MaxPooling1D(1))
#model.add(layers.Conv1D(64, 1, activation='relu'))
#model.add(layers.MaxPooling1D(1))
#model.add(layers.Flatten())
#model.add(layers.Dense(16, activation='softmax'))
#model.add(Dense(nb_actions, activation='linear'))

##########################################################
####### two state simulation learning ...
#model = Sequential()
#model.add(Flatten(input_shape=(1,) + env.observation_space.shape))
#model.add(Dense(256))
#model.add(Activation('relu'))
#model.add(Dense(512))
#model.add(Activation('relu'))
#model.add(Dense(1024))
#model.add(Activation('relu'))
#model.add(Dense(512))
#model.add(Activation('relu'))
#model.add(Dense(256))
#model.add(Activation('relu'))
#model.add(Dense(128))
#model.add(Activation('relu'))
#model.add(Dense(nb_actions, activation='linear'))

print(model.summary())

# Finally, we configure and compile our agent. You can use every built-in Keras optimizer and
# even the metrics!
memory = SequentialMemory(limit=500000, window_length=1)#window is related with the number of actions
TempSeqMemory = TempSequentialMemory(limit=500000, window_length=1)
policy = EpsGreedyQPolicy()
#policy = BoltzmannQPolicy()
#policy = SoftmaxPolicy()
#policy = GreedyQPolicy()
#policy = LinearAnnealedPolicy()
#policy = BoltzmannGumbelQPolicy()
# enable the dueling network
# you can specify the dueling_type to one of {'avg','max','naive'}
dqn = AIMMUAgent(env, TempSeqMemory, model=model, nb_actions=nb_actions, memory=memory, nb_steps_warmup=100,
               enable_dueling_network=True, dueling_type='max', target_model_update=1e-2, policy=policy)
dqn.compile(Adam(lr=1e-3), metrics=['mae'])

# Okay, now it's time to learn something! We visualize the training here for show, but this
# slows down training quite a lot. You can always safely abort the training prematurely using
# Ctrl + C.
#dqn.fit(env, nb_steps=10000, visualize=False, verbose=2)

# After training is done, we save the final weights.
#dqn.save_weights('ai_mmu_{}_weights.h5f'.format(ENV_NAME), overwrite=True)

weights_filename = 'ai_mmu_{}_weights.h5f'.format(ENV_NAME)

dqn.load_weights(weights_filename)
#env.plot_accuracy()
# Finally, evaluate our algorithm for 5 episodes.
env.get_file_list(False)
dqn.test(env, nb_episodes=5, visualize=False)
