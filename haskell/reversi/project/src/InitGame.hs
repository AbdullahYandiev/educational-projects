module InitGame 
  ( initGame  
  ) where

import Types
import Board
import Consts

-- Initialization of the game: White starts odd games, Black - even
initGame :: Int -> GameWorld
initGame gameNum = 
  World  initBoard
         startScore
         startPlayer
         False
         False
         gameNum
  where
    startPlayer = if odd gameNum then White else Black 