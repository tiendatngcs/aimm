import logging
from gym.envs.registration import register

logger = logging.getLogger(__name__)

register(
    id='memenv-v0',
    entry_point='gym_memenv.envs:MemEnv',
)
