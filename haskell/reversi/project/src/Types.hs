module Types where

import Data.Array

-- Coordinate of square on a board
type Coordinate = (Int, Int)

-- Score of the game: (white, black)
type Score = (Int, Int)

-- A player is either White or Black
data Player = White | Black deriving (Eq, Show)

-- Define the state of each square on a board: empty or player's button placed on it
type BoardSquare = Maybe Player

-- Game board model: 2d-array
type Board = Array Coordinate BoardSquare

-- General game state
data GameWorld = World {
  gameBoard         ::  Board,
  score             ::  Score,
  playerTurn        ::  Player,
  passedOnLastTurn  ::  Bool,
  bothStalled       ::  Bool,
  gameNumber        ::  Int
}
