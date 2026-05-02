# Pacman

A fully-playable Pacman clone built with **OpenGL / GLUT (freeglut)** in C++,
submitted as our **Computer Graphics Lab Final** group project.

---

## Features

| Feature | Details |
|---|---|
| Classic maze | 28 × 31 tile map with walls, dots, and power pellets |
| Pacman | Smooth movement, animated chomping mouth, lives system |
| 4 Ghosts | Blinky (red), Pinky (pink), Inky (cyan), Clyde (orange) |
| Ghost AI | Chase → Scatter cycling, Frightened mode (power pellet), Eaten mode (return home) |
| Warp tunnel | Left/right edge wrap-around |
| Score & HUD | Dot / ghost / power-pellet scoring, remaining lives display |
| Game states | Intro screen, READY countdown, Play, Pause, Death animation, Win, Game Over |

---

## Controls

| Key | Action |
|---|---|
| **Arrow Keys** | Move Pacman |
| **P** | Pause / Resume |
| **R** | Restart |
| **ESC / Q** | Quit |

---

## Build & Run

### Prerequisites (Ubuntu / Debian)

```bash
sudo apt-get install -y freeglut3-dev libgl-dev libglu1-mesa-dev
```

### Compile

```bash
make
```

### Run

```bash
./pacman
```

### Clean

```bash
make clean
```

---

## Project Structure

```
Pacman/
├── pacman.cpp   # Full game source (OpenGL/GLUT)
├── Makefile     # Build configuration
└── README.md    # This file
```

---

## Scoring

| Item | Points |
|---|---|
| Dot | 10 |
| Power Pellet | 50 |
| Ghost (1st) | 200 |
| Ghost (2nd) | 400 |
| Ghost (3rd) | 800 |
| Ghost (4th) | 1600 |

