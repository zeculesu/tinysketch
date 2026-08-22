ThisBuild / scalaVersion := "2.13.18"

lazy val root = (project in file("."))
  .settings(
    name := "tinysketch",
    libraryDependencies +=
      "org.chipsalliance" %% "chisel" % "7.6.0",
    Compile / run / fork := true
  )

addCompilerPlugin(
  "org.chipsalliance" % "chisel-plugin" % "7.6.0" cross CrossVersion.full
)