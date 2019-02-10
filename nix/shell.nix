{ pkgs ? import <nixpkgs> { } }:

with pkgs; stdenv.mkDerivation {
  name = "projectm-shell";
  buildInputs = [
    glm
    libGLU_combined
    SDL2
    darwin.apple_sdk.frameworks.Carbon
  ];
}
