{-# LANGUAGE CPP #-}
{-# LANGUAGE NoRebindableSyntax #-}
{-# OPTIONS_GHC -fno-warn-missing-import-lists #-}
module Paths_project_template (
    version,
    getBinDir, getLibDir, getDynLibDir, getDataDir, getLibexecDir,
    getDataFileName, getSysconfDir
  ) where

import qualified Control.Exception as Exception
import Data.Version (Version(..))
import System.Environment (getEnv)
import Prelude

#if defined(VERSION_base)

#if MIN_VERSION_base(4,0,0)
catchIO :: IO a -> (Exception.IOException -> IO a) -> IO a
#else
catchIO :: IO a -> (Exception.Exception -> IO a) -> IO a
#endif

#else
catchIO :: IO a -> (Exception.IOException -> IO a) -> IO a
#endif
catchIO = Exception.catch

version :: Version
version = Version [0,1,0,0] []
bindir, libdir, dynlibdir, datadir, libexecdir, sysconfdir :: FilePath

bindir     = "/Users/YandievAbdullax/Desktop/haskell/reversi/.stack-work/install/x86_64-osx/57733e85c2b7da499eb2586afa28cce485802f8f82ffafe877df33aaeb7acce9/8.10.7/bin"
libdir     = "/Users/YandievAbdullax/Desktop/haskell/reversi/.stack-work/install/x86_64-osx/57733e85c2b7da499eb2586afa28cce485802f8f82ffafe877df33aaeb7acce9/8.10.7/lib/x86_64-osx-ghc-8.10.7/project-template-0.1.0.0-DUTjc2ej1IJK8x0hFLPAyI"
dynlibdir  = "/Users/YandievAbdullax/Desktop/haskell/reversi/.stack-work/install/x86_64-osx/57733e85c2b7da499eb2586afa28cce485802f8f82ffafe877df33aaeb7acce9/8.10.7/lib/x86_64-osx-ghc-8.10.7"
datadir    = "/Users/YandievAbdullax/Desktop/haskell/reversi/.stack-work/install/x86_64-osx/57733e85c2b7da499eb2586afa28cce485802f8f82ffafe877df33aaeb7acce9/8.10.7/share/x86_64-osx-ghc-8.10.7/project-template-0.1.0.0"
libexecdir = "/Users/YandievAbdullax/Desktop/haskell/reversi/.stack-work/install/x86_64-osx/57733e85c2b7da499eb2586afa28cce485802f8f82ffafe877df33aaeb7acce9/8.10.7/libexec/x86_64-osx-ghc-8.10.7/project-template-0.1.0.0"
sysconfdir = "/Users/YandievAbdullax/Desktop/haskell/reversi/.stack-work/install/x86_64-osx/57733e85c2b7da499eb2586afa28cce485802f8f82ffafe877df33aaeb7acce9/8.10.7/etc"

getBinDir, getLibDir, getDynLibDir, getDataDir, getLibexecDir, getSysconfDir :: IO FilePath
getBinDir = catchIO (getEnv "project_template_bindir") (\_ -> return bindir)
getLibDir = catchIO (getEnv "project_template_libdir") (\_ -> return libdir)
getDynLibDir = catchIO (getEnv "project_template_dynlibdir") (\_ -> return dynlibdir)
getDataDir = catchIO (getEnv "project_template_datadir") (\_ -> return datadir)
getLibexecDir = catchIO (getEnv "project_template_libexecdir") (\_ -> return libexecdir)
getSysconfDir = catchIO (getEnv "project_template_sysconfdir") (\_ -> return sysconfdir)

getDataFileName :: FilePath -> IO FilePath
getDataFileName name = do
  dir <- getDataDir
  return (dir ++ "/" ++ name)
