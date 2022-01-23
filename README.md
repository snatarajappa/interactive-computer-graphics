{\rtf1\ansi\ansicpg1252\cocoartf2636
\cocoatextscaling0\cocoaplatform0{\fonttbl\f0\fswiss\fcharset0 Helvetica;\f1\fnil\fcharset0 HelveticaNeue-Bold;\f2\fnil\fcharset0 AndaleMono;
\f3\fmodern\fcharset0 Courier;\f4\fnil\fcharset0 Menlo-Regular;}
{\colortbl;\red255\green255\blue255;\red220\green161\blue13;\red47\green255\blue18;\red0\green0\blue0;
\red26\green26\blue26;\red237\green237\blue237;\red0\green0\blue0;}
{\*\expandedcolortbl;;\cssrgb\c89412\c68627\c3922;\cssrgb\c15686\c99608\c7843;\csgray\c0\c90000;
\cssrgb\c13333\c13333\c13333;\cssrgb\c94510\c94510\c94510;\csgray\c0;}
\margl1440\margr1440\vieww30040\viewh18900\viewkind0
\pard\tx720\tx1440\tx2160\tx2880\tx3600\tx4320\tx5040\tx5760\tx6480\tx7200\tx7920\tx8640\pardirnatural\partightenfactor0

\f0\fs24 \cf0 Xcode --> Version 13.2.1 (13C100)\
MacOS --> 12.01\
\
With this configuration I found that we cannot use code written for intel x86_64 won't work\
Hence I had to make below configuration changes\
\
\
Go to build settings --> Targets --> \
Build Active Arch Only - No\
Exclude Arch --> arm64\
\
\
Then I had to follow {\field{\*\fldinst{HYPERLINK "https://www.wisdomgeek.com/development/installing-intel-based-packages-using-homebrew-on-the-m1-mac/"}}{\fldrslt 
\f1\b\fs40 \cf2 https://www.wisdomgeek.com/development/installing-intel-based-packages-using-homebrew-on-the-m1-mac/}}
\f1\b\fs40 \cf2 \
\
\
to install rosetta and brew for x86_64\
\
\
Then I installed glew and glfw to be compatible with x86_64\
\
Anyway cyCodeBase works with x86_64\
\
\
\
\
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\pardirnatural\partightenfactor0

\f2\b0\fs24 \cf3 \cb4 \CocoaLigature0 arch -x86_64 /usr/local/HomeBrew/bin/brew install glfw\
\
\
arch -x86_64 /usr/local/HomeBrew/bin/brew install glew\
\
arch -x86_64 /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install.sh)"\
\
\
\
\
\
\
This is for ARM64\
\
\
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"   \
\
\
eval "$(/opt/homebrew/bin/brew shellenv)"\
\
echo 'eval "$(/opt/homebrew/bin/brew shellenv)"' >> /Users/snatarajappa/.zprofile  \
\
\
\
\
\

\f1\b\fs40 \cf2 \cb1 \CocoaLigature1 \
\pard\tx720\tx1440\tx2160\tx2880\tx3600\tx4320\tx5040\tx5760\tx6480\tx7200\tx7920\tx8640\pardirnatural\partightenfactor0
\cf2 \
\pard\pardeftab720\partightenfactor0

\f3\b0\fs34 \cf5 \cb6 \expnd0\expndtw0\kerning0
/usr/sbin/softwareupdate --install-rosetta\
\
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"\
\
\
\
\
arch -x86_64 /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install.sh)"\
\
\
\
arch -x86_64 /usr/local/homebrew/bin/brew install <package name>\
\
\
/opt/homebrew/bin/brew install <package name>\
\
arch -x86_64 zsh\
\
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\pardirnatural\partightenfactor0

\f4\fs22 \cf7 \cb1 \kerning1\expnd0\expndtw0 \CocoaLigature0 clang++  HelloWorld/main.cpp lib/libglut.3.11.0.dylib -framework OpenGL -I include -L lib -arch x86_64 -o run\
\
./run
\f1\b\fs40 \cf2 \CocoaLigature1 \
\pard\tx720\tx1440\tx2160\tx2880\tx3600\tx4320\tx5040\tx5760\tx6480\tx7200\tx7920\tx8640\pardirnatural\partightenfactor0

\f0\b0\fs24 \cf0 \
}