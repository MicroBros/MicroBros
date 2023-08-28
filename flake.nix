{
  description = "MicroBros";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-compat = {
      url = "github:edolstra/flake-compat";
      flake = false;
    };
    flake-parts.url = "github:hercules-ci/flake-parts";
  };

  outputs = {
    self,
    nixpkgs,
    flake-parts,
    ...
  } @ inputs:
    flake-parts.lib.mkFlake {inherit inputs;} {
      systems = ["x86_64-linux"];

      perSystem = {
        config,
        self',
        inputs',
        pkgs,
        system,
        ...
      }: let
        pkgs = import nixpkgs {
          inherit system;
        };
      in {
        devShells.default = with pkgs;
          pkgs.mkShell rec {
            name = "microbros";
            packages = with xorg; [
              cmake
              gcc-arm-embedded
              libX11
              libICE
              libXi
              libXScrnSaver
              libXcursor
              libXinerama
              libXext
              libXrandr
              libXxf86vm
              libXfixes
              libXrender
              xorgproto
              dbus
              libdecor
              libpulseaudio
              pipewire
              udev
              libxkbcommon
              libiconv
              libGL
            ];

            shellHook = ''
              export LD_LIBRARY_PATH="${lib.makeLibraryPath packages}:${stdenv.cc.cc.lib}/lib64:$LD_LIBRARY_PATH"
              export LIBRARY_PATH="${lib.makeLibraryPath packages}:${stdenv.cc.cc.lib}/lib64"
              export PKG_CONFIG_EXECUTABLE="${pkgs.pkg-config}/bin/pkg-config"
            '';
          };
      };
    };
}
