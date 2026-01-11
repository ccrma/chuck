{
  description = "ChucK strongly-timed audio programming language";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs = { self, nixpkgs }:
    let
      systems = nixpkgs.lib.systems.flakeExposed;
      forAllSystems = f:
        nixpkgs.lib.genAttrs systems (system:
          let
            pkgs = import nixpkgs { inherit system; };
            nativeTools = with pkgs; [ pkg-config flex bison ];
            linuxLibs = pkgs.lib.optionals pkgs.stdenv.isLinux
              (with pkgs; [ libsndfile alsa-lib pulseaudio jack2 ]);
          in
          f pkgs nativeTools linuxLibs);
      version = "1.5.5.7-dev";
    in
    {
      packages = forAllSystems (pkgs: nativeTools: linuxLibs: {
        default = pkgs.stdenv.mkDerivation {
          pname = "chuck";
          inherit version;
          src = ./.;

          nativeBuildInputs = nativeTools;
          buildInputs = linuxLibs;

          enableParallelBuilding = true;

          buildPhase = ''
            cd src
            make linux-all
          '';

          installPhase = ''
            install -Dm755 src/chuck $out/bin/chuck
            install -Dm644 README.md $out/share/doc/chuck/README.md
          '';

          meta = with pkgs.lib; {
            description = "Strongly-timed audio programming language";
            homepage = "https://chuck.stanford.edu/";
            license = [ licenses.gpl2Plus licenses.mit ];
            mainProgram = "chuck";
            platforms = platforms.linux;
          };
        };
      });

      devShells = forAllSystems (pkgs: nativeTools: linuxLibs: {
        default = pkgs.mkShell {
          packages = nativeTools ++ linuxLibs ++ (with pkgs; [ python3 nodejs_24 protobuf ]);
        };
      });

      formatter = forAllSystems (pkgs: _: _: pkgs.nixfmt-rfc-style);
    };
}
