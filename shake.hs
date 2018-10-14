import Development.Shake
import Development.Shake.Command
import Development.Shake.FilePath
import Development.Shake.Util

import Data.List
import System.Directory
import qualified System.Directory as D
import System.Environment
import Control.Monad
import System.IO
import Control.Monad.State

dropDir :: FilePath -> FilePath -> FilePath
dropDir x base = joinPath $ dropDir' (splitPath x) (splitPath base)
  where
    dropDir' :: [FilePath] -> [FilePath] -> [FilePath]
    dropDir' xs [] = xs
    dropDir' (x:xs) (b:bs) =
      let diff = x \\ b
      in if diff == "" || diff == "/" then
          dropDir' xs bs
        else
          error $ "could not drop '"++(joinPath (b:bs))++"' from '"++(joinPath (x:xs))++"'"

listDirRec :: FilePath -> IO [FilePath]
listDirRec path = do
  isDir <- D.doesDirectoryExist path
  if isDir then do
    files <- ((path </>) <$>) <$> listDirectory path
    concat <$> traverse listDirRec files
  else
    return [path]

shakeOptions' :: ShakeOptions
shakeOptions' = shakeOptions{
  shakeFiles = "bld"</>"shake"</>"db",
  shakeReport = (("bld"</>"rep") </>) <$> [
    "t"<.>"trace", "h"<.>"html"
  ],
  shakeLint = Just LintBasic,
  shakeTimings = True
}

main :: IO ()
main = shakeArgs shakeOptions' $ do
  let path_in = "src"
  let path_lib = "lib"
  let path_bld = "bld"
  let path_dist = "dist"
  let path_dep = path_bld</>"dep"
  let path_compdb = path_bld</>"compdb"

  liftIO $ createDirectoryIfMissing True (path_bld</>"rep")

  compilerFromEnv <- liftIO $ lookupEnv "CXX"
  let compiler =
        case compilerFromEnv of
          Just x -> if null x then "clang++" else x
          Nothing -> "clang++"

  let compdb = path_bld</>"compile_commands.json"

  compdb %> \out -> do
    files <- liftIO $ listDirRec path_compdb
    () <- cmd Shell ["sed", "-e", "'1s/^/[\\\n/'", "-e", "'$s/,$/\\\n]/'"] files [">", out]
    return ()

  let cppObjectPathFor name src = path_bld<//>name<//>src<.>"o"
  let cppObjectDepPathFor name src = path_dep<//>name<//>src<.>"dep"
  let cppObjectCompdbPathFor name src = path_compdb<//>name<//>src<.>"json"
  let cppObjectsRuleFor name systemIncludes =
        [cppObjectPathFor name "*", cppObjectDepPathFor name "*", cppObjectCompdbPathFor name "*"] &%> \[out, dep, compdb] -> do
          let src = path_in</>(dropDir out path_bld)-<.>"cpp"
          --  "-Ofast"
          let diagFlags = ["-fcolor-diagnostics"]
          let warnFlags = ["-Weverything", "-Wno-c++98-compat", "-Wno-c++98-c++11-compat", "-Wno-float-equal", "-Wno-c++98-c++11-compat-pedantic", "-Wno-c99-extensions", "-Wno-c++98-c++11-c++14-compat", "-Wno-padded"]
          let sanitizeFlags = []
          let outputFlags = ["-o", out]
          let includeFlags = ("-isystem"++) <$> ["/usr/local/opt/llvm/include"]++systemIncludes
          let otherFlags = ["-std=c++1z", "-g"]
          let command = [compiler, "-O0"] ++ diagFlags ++ warnFlags ++ sanitizeFlags ++ outputFlags ++ includeFlags ++ otherFlags

          () <- cmd command "-M" "-MF" [dep] [src]
          needMakefileDependencies dep

          () <- cmd command "-MJ" [compdb] "-c" [src]
          return ()

  let localLibNameToPath name = path_dist<//>"dbg"<//>"lib"++name<.>"dylib"
  let cppDynamicLibRule name linkDeps srcs =
        localLibNameToPath name %> \out -> do
          let srcs' = cppObjectPathFor name <$> srcs
          need srcs'

          let libFlags = ("-l"++) <$> []++linkDeps
          let sanitizeFlags = [""]
          () <- cmd compiler "-shared" sanitizeFlags "-g" "-rdynamic" "-install_name" ("@rpath/lib"++name++".dylib") "-O0" "-o" [out] libFlags "-L/usr/local/opt/llvm/lib" srcs'

          return ()

  let localExecNameToPath name = path_dist<//>"dbg"<//>name
  let cppExecRule name systemLinkDeps localLinkDeps linkSearchPaths srcs =
        localExecNameToPath name %> \out -> do
          need $ localLibNameToPath <$> localLinkDeps

          let srcs' = cppObjectPathFor name <$> srcs
          need srcs'

          let libFlags = ("-l"++) <$> []++systemLinkDeps++localLinkDeps
          let libSearchPaths = []++(takeDirectory <$> localLibNameToPath <$> localLinkDeps)++linkSearchPaths
          let libSearchFlags = ("-L"++) <$> libSearchPaths

          () <- cmd "clang++" "-rpath" (intersperse ";" libSearchPaths) "-O0" "-o" [out] libFlags libSearchFlags "-L/usr/local/opt/llvm/lib" srcs'
          () <- cmd "dsymutil" [out]
          return ()

  cppObjectsRuleFor "app" ["/usr/local/include/SDL2", path_lib<//>"rolmodl"<//>"hpp"]
  cppExecRule "app" ["sdl2", "rolmodl"] [] [path_lib<//>"rolmodl"] $ [
        "main",
        "updown",
        "Interval",
        "BoolInterval",
        "DTInterval"
      ]

  cppObjectsRuleFor "tst" ["/usr/local/include/SDL2", path_lib<//>"rolmodl"<//>"hpp"]
  cppExecRule "tst" ["sdl2", "rolmodl"] [] [path_lib<//>"rolmodl"] $ []

  want [localExecNameToPath "app"{-, localExecNameToPath "tst"-}]
