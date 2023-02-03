module Game
  ( runGame
  ) where

import Draw
import Consts
import Events
import InitGame

import Graphics.Gloss.Interface.Pure.Game

-- Running game function (unpure)
runGame :: IO ()
runGame = do
  play   display
         boardColor
         fps
         (initGame 1)
         drawGame
         handleEvent
         updateGame
