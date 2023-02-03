module Draw
  ( drawGame
  ) where

import Types
import Board
import Moves
import Consts

import Data.Maybe
import Data.Array
import Graphics.Gloss.Interface.Pure.Game

-- Draw a picture: game grid, text with player turn and score ------------
drawGame :: GameWorld -> Picture
drawGame world = Translate 
  ( fromIntegral lowerLeftCornerX ) 
  ( fromIntegral lowerLeftCornerY ) 
  $ Pictures [ drawGrid
             , drawDisks world
             , drawTurnText world
             , drawGameOver world
             , drawScore world
             ]
--------------------------------------------------------------------------

-- Draw a game grid ------------------------------------------------------
drawGrid :: Picture
drawGrid = Pictures $ mapBoardSquares drawSquare

-- Draw a square according to a given coordinate
drawSquare :: Coordinate -> Picture
drawSquare (coordX, coordY) = drawBox 
  (fromIntegral $ gridAbsoluteLeftX + squareSize * coordX
  ,fromIntegral $ gridAbsoluteLeftY + squareSize * coordY) 
  (fromIntegral $ gridAbsoluteLeftX + squareSize * (coordX + 1)
  ,fromIntegral $ gridAbsoluteLeftY + squareSize * (coordY + 1))

-- A helper function that draw a box
drawBox :: (Float, Float) -> (Float, Float) -> Picture
drawBox (startX, startY) (endX, endY) = Color gridColor (Line path)
  where
    path = [(startX, startY), (endX, startY), (endX, endY), (startX, endY), (startX, startY)]
--------------------------------------------------------------------------

-- Draw player disks which are currently on a game board -----------------
drawDisks :: GameWorld -> Picture
drawDisks world = Pictures 
  [Translate diskPosX
             diskPosY
           $ Color (colorForDisk $ snd placedSquare) 
                   (circleSolid circleSize)
  | placedSquare <- placedSquares
  , let (diskPosX, diskPosY) = gridPosForDisks $ fst placedSquare]
  where 
    placedSquares = filter (isJust . snd) (assocs . gameBoard $ world)

-- Function that converts given coordinate to a window point (centre of square)
gridPosForDisks :: Coordinate -> (Float, Float)
gridPosForDisks (coordX, coordY) = (actualX, actualY) 
  where
    actualX = fromIntegral $ gridAbsoluteLeftX 
            + coordX * squareSize + squareSize `div` 2
    actualY = fromIntegral $ gridAbsoluteLeftY 
            + (gridSize - coordY - 1) * squareSize + squareSize `div` 2

-- Function that converts given placed button to a player disk color
colorForDisk :: BoardSquare -> Color
colorForDisk (Just White) = white
colorForDisk _ = black
--------------------------------------------------------------------------

-- Draw a player color whose turn it is to move
drawTurnText :: GameWorld -> Picture
drawTurnText world =
  Scale 0.15 0.15 $
  Color textColor $
  Translate turnTextPositionX turnTextPositionY $
  Text currentTurnText
    where
      currentTurnText
        | noMovesAvailableForBoth currentBoard = ""
        | null $ movesAvailableForPlayer currentBoard currentTurn = 
            "No moves available for " ++ player ++ ", turn for " ++ opponent
        | otherwise = "Turn for " ++ player
      currentBoard = gameBoard world
      currentTurn = playerTurn world
      player = show currentTurn
      opponent = show $ opposingPlayer currentTurn
--------------------------------------------------------------------------

-- Draw a picture when game is over: winner, score etc. ------------------
drawGameOver :: GameWorld -> Picture
drawGameOver world
  | noMovesAvailableForBoth board = Pictures
    [ Color bgColor $
      Scale 2 2 $
      rectangleSolid (fromIntegral windowSizeX) (fromIntegral windowSizeY)
    , Color textColor $ 
      Translate gameOverTextPositionX gameOverTextPositionY $
      Scale 0.5 0.5 $
      Text gameOverText
    , Color textColor $
      Translate winnerTextPositionX winnerTextPositionY $
      Scale 0.5 0.5 $
      Text winnerText
    , Color textColor $
      Translate drawTextPositionX drawTextPositionY $
      Scale 0.5 0.5 $
      Text gameDrawText 
    , Color textColor $ 
      Translate tryAgainTextPositionX tryAgainTextPositionY $
      Scale 0.3 0.3 $
      Text tryAgainText
    ]
  | otherwise = Blank
    where
      board = gameBoard world
      (whiteScore, blackScore) = score world
      winnerText
        | whiteScore > blackScore = "White wins"
        | whiteScore < blackScore = "Black wins"
        | otherwise = ""
      gameDrawText
        | whiteScore == blackScore = "Draw"
        | otherwise = ""
--------------------------------------------------------------------------

-- Draw a corrent score of the game --------------------------------------
drawScore :: GameWorld -> Picture
drawScore world = Pictures
  [ Color white $
    Translate whiteScoreDiskPosX whiteScoreDiskPosY $
    circleSolid scoreCircleSize
  , Color black $
    Translate blackScoreDiskPosX blackScoreDiskPosY $
    circleSolid scoreCircleSize
  , Scale 0.7 0.7 $
    Color white $
    Translate whiteScoreTextPosX whiteScoreTextPosY $
    Text $ show whiteScore
  , Scale 0.7 0.7 $
    Color black $
    Translate blackScoreTextPosX blackScoreTextPosY $
    Text $ show blackScore
  ]
  where
    (whiteScore, blackScore) = score world
--------------------------------------------------------------------------
