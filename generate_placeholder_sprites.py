#!/usr/bin/env python3
import os
from PIL import Image, ImageDraw

def create_sprite(filename, size, color):
    os.makedirs(os.path.dirname(filename), exist_ok=True)
    img = Image.new('RGBA', size, color)
    img.save(filename)
    print(f"Created placeholder: {filename}")

# Create all placeholder sprites
sprites = [
    # Player sprites (yellow variations)
    ("resources/sprites/player/idle.png", (20, 20), (255, 255, 0, 255)),
    ("resources/sprites/player/walk_up.png", (20, 20), (255, 255, 100, 255)),
    ("resources/sprites/player/walk_down.png", (20, 20), (255, 255, 100, 255)),
    ("resources/sprites/player/walk_left.png", (20, 20), (255, 255, 100, 255)),
    ("resources/sprites/player/walk_right.png", (20, 20), (255, 255, 100, 255)),
    ("resources/sprites/player/digging.png", (20, 20), (255, 200, 0, 255)),
    ("resources/sprites/player/shield.png", (20, 20), (255, 255, 255, 255)),
    
    # Monster sprites
    ("resources/sprites/monsters/red_idle.png", (20, 20), (255, 0, 0, 255)),
    ("resources/sprites/monsters/red_walk.png", (20, 20), (255, 50, 50, 255)),
    ("resources/sprites/monsters/red_angry.png", (20, 20), (200, 0, 0, 255)),
    ("resources/sprites/monsters/dragon_idle.png", (20, 20), (0, 255, 0, 255)),
    ("resources/sprites/monsters/dragon_walk.png", (20, 20), (50, 255, 50, 255)),
    ("resources/sprites/monsters/dragon_breath.png", (20, 20), (0, 200, 0, 255)),
    
    # Weapons
    ("resources/sprites/weapons/harpoon_h.png", (20, 4), (150, 150, 150, 255)),
    ("resources/sprites/weapons/harpoon_v.png", (4, 20), (150, 150, 150, 255)),
    ("resources/sprites/weapons/chain.png", (20, 20), (100, 100, 100, 255)),
    
    # Effects
    ("resources/sprites/effects/explosion1.png", (30, 30), (255, 100, 0, 255)),
    ("resources/sprites/effects/explosion2.png", (30, 30), (255, 150, 0, 255)),
    ("resources/sprites/effects/explosion3.png", (30, 30), (255, 200, 0, 255)),
    ("resources/sprites/effects/explosion4.png", (30, 30), (255, 255, 0, 255)),
    ("resources/sprites/effects/sparkle.png", (10, 10), (255, 255, 255, 255)),
    
    # Environment
    ("resources/sprites/environment/dirt.png", (20, 20), (139, 69, 19, 255)),
    ("resources/sprites/environment/rock.png", (20, 20), (128, 128, 128, 255)),
    ("resources/sprites/environment/tunnel.png", (20, 20), (0, 0, 0, 255)),
    
    # UI
    ("resources/sprites/ui/speed.png", (16, 16), (0, 255, 255, 255)),
    ("resources/sprites/ui/range.png", (16, 16), (0, 255, 0, 255)),
    ("resources/sprites/ui/rapid.png", (16, 16), (255, 0, 255, 255)),
    ("resources/sprites/ui/shield.png", (16, 16), (255, 255, 0, 255)),
]

for filename, size, color in sprites:
    create_sprite(filename, size, color)

print("\nPlaceholder sprites created successfully!")
print("Replace these with actual game sprites when available.")
