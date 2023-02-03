module Events
  ( handleEvent
  , updateGame
  ) where

import Types
import Moves
import Board
import Consts
import InitGame

import Data.Maybe
import Graphics.Gloss.Interface.Pure.Game

-- Handle input events
handleEvent :: Event -> GameWorld -> GameWorld
handleEvent (EventKey (SpecialKey KeySpace) Down _ _) world
  | noMovesAvailableForBoth $ gameBoard world = initGame nextGameNumber
  | otherwise = world
  where 
    nextGameNumber = (gameNumber world) + 1
handleEvent (EventKey (MouseButton LeftButton) Down _ clickPosition) world
  -- Both players have passed -> Game Over
  | bothStalled world = world
  -- No valid position: miss the board grid
  | isNothing clickCoordinate = world
  -- Player has no moves available -> turn moves to an opponent and check click
  | null $ movesAvailableForPlayer board player = 
      checkPlayerTurn 
        world { playerTurn = opponent
              , passedOnLastTurn = True 
              , bothStalled = passedOnLastTurn world } $
        fromJust clickCoordinate
  -- There is available moves a right click position: check it
  | otherwise = checkPlayerTurn world $ fromJust clickCoordinate
    where
      board = gameBoard world
      player = playerTurn world
      opponent = opposingPlayer player
      clickCoordinate = getClickCoordinate clickPosition
handleEvent _ world = world 

-- Function that checks the move and applies it, if it is possible
checkPlayerTurn :: GameWorld -> Coordinate -> GameWorld
checkPlayerTurn world coordinate
  -- No valid moves
  | null movesOnClick = world
  -- Apply a move and change score and turn to the opposing player, also reset any pass counters
  | otherwise = World newBoard newScore opponent False False gameNum
    where
      board = gameBoard world
      player = playerTurn world
      opponent = opposingPlayer player
      gameNum = gameNumber world 
      (whiteScore, blackScore) = score world
      movesOnClick = getMovesOnClick board player coordinate
      newDisksNumber = length movesOnClick 
      newBoard = applyMove board player movesOnClick
      newScore
        | player == White = (whiteScore + newDisksNumber, blackScore - newDisksNumber + 1)
        | otherwise    =    (whiteScore - newDisksNumber + 1, blackScore + newDisksNumber)

-- Function that tries to locate the coordinates that mouse did click
getClickCoordinate :: (Float, Float) -> Maybe Coordinate
getClickCoordinate (clickX, clickY)
  | coordinateX < 0 || coordinateX >= gridSize = Nothing
  | coordinateY < 0 || coordinateY >= gridSize = Nothing
  | otherwise = Just (coordinateX, coordinateY)
  where
    coordinateX = translatedClickX `div` squareSize 
    coordinateY = (gridSize - 1) - translatedClickY `div` squareSize
    translatedClickX = (round clickX) - lowerLeftCornerX - gridAbsoluteLeftX
    translatedClickY = (round clickY) - lowerLeftCornerY - gridAbsoluteLeftY

-- Simulation step (not use)
updateGame :: Float -> GameWorld -> GameWorld
updateGame _ world = world
