*I made a game!*

#Creative Title (I'm standing by that name) 
is a split screen side by side bullet hell shooter.
The goal is to send bullets over to your opponent's side of the screen and overwhelm them until they run out of lives.
Note: to play this game, you need a controller.
I used a PS3 controller with the MotionInJoy drivers.
Other controllers should work, but you need to remap them.
The mapping can be found in Game.cpp in the #define section


#Controls:
##Keyboard/Player 1:
Z: Select/Player 1 shoot
X (Keyboard)/O(PS3 Controller): Cancel/Back/Charge Super
Directional Keys: Player 1 Move
Left-Shift: Player 1 Focus Movement + Defensive Super


##Controller/Player 2:
X (PS3 Controller): Select/Player 2 shoot
PS3 D-Pad: Player 2 Move
R1 (PS3 Controller): Player 2 Focus Movement + Defensive Special
L1(PS3 Controlelr): Player 2 Charge Super
O (PS3 Controller): Cancel/Back

#Basic Gameplay:
Hold your shot button for continuous fire, and try to hit the enemies!
When you kill an enemy, they will explode, destroying all small bullets in a small area.
A small handful of bullets plus any destroyed bullets will appear at the top of your opponent's screen.
Enemy spawn and firing rates will increase as the game goes on, so don't dilly-dally!
When you get hit, you will be invincible for a while.
After that wears off, you will destroy all bullets and offensive skills in a large area.

#Focused Movement:
Holding your focus button (Left-Shift or R1) will invoke focused movement.
During focused movement, you move slower, allowing for more precise adjustments.
Additionally, a small white square will appear on your character.  That is your hitbox.
Additionally, focused movement will activate your character's defensive special.
The defensive special will deactivate once the button is released.

#Scoring:
Points are obtained by killing enemies or destroying bullets (by means other than getting hit).
A small bonus multiplier is applied by getting points often in a short amount of time.
Therefore, a good point gathering strategy is to lets enemies build up, and firing away to land continuous kills.
After you get enough points, you will send over your character's offensive special!
Different characters have different point requirements.
Stronger offensives require much higher scoring combos.
Scoring points also fills up your bar, allowing you to use supers!

#Supers:
At the bottom of your screen is a small gauge that fills up as you score.
A super has three levels, corresponding to the three sections in the gauge.
Holding your charge button (X in keyboard, O in controller) will cause you to charge your super.
Your gauge will fill with orange and you will hear a small sound when your charge reaches certain levels.
When you are satisfied with your charge, release the charge button to use your super.
The longer you charge your super, the more powerful and expensive it will be.
Charging your super takes a while, so know when you need it in advance.
A super can have up to three levels:
Level 1: Destroys all small bullets in a small area.  Grants brief invincibility.
Level 2: Destroys all bullets in a medium area.  Grants invincibility.
Level 3: Destroys all bullets and supers in a massive area.  Grants prolonged invincibility.
All bullets destroyed by your super contribute to your points, and therefore your offensive specials.
However, points obtained this way will NOT contribute to your charge (that would be circular and overpowered).

#Apocalypse:
While the first 6 stages are mechanically similar, Apocalypse is a special seventh stage.
Both players begin with 20 lives, instead of 5.
At the beginning, a boss will spawn on both sides of the screen.
Hitting the boss will grant points and send bullets to your opponent's side of the screen.
The boss will regularly spawn regular enemies that must also be dealt with.
Additionally, the boss will regularly attack with offensive specials.
The boss can spawn different specials at the same time, and some combinations are especially deadly together!
The attack patterns will get increasingly difficult over time.
During apocalypse, a gauge will appear at the top of the screen.  It will fill up over time.
WHen it's full, the real challenge starts.
You can work together to survive, or try to push your opponent into oblivion.
Do your best to survive the end of the world!

#Characters:
Each character has a unique offensive and defensive skill.

##Corvus:
Time Crunch: A field around Corvus slows down all nearby bullets.
Time Dilation: A magic clock on the field temporarily speeds up small bullets within the circle.

##Ariel:
Holy Orb: A slow but powerful shot that plows through enemies.  Great for clearing crowded fields
Cross Fire: A cross appears on the screen that periodically shoots bullets in four directions.

##Flora:
Flower Power: Fires bullets in eight directions at a slightly decreased firing rate.
Blooming Bullet: A flower blooms on the opponent's screen.  After a pause, it shoots bullets in 6 directions.

##Thor:
Dominion: Thor is immobile while focused, and movement will move a targeting reticle.  Release the focus button to teleport Thor to the targeting reticle.
Judgment: A magic field that redirects nearby large bullets toward the opponent.

##Crystal:
Crystal Cutter: A small 3D crystal revolves around Crystal, destroying any small bullets it comes into contact with.
Crystal Clear: Large gemstones obscure the opponent's field.  Hiding bullets, enemeies, and other offensive specials.

##Kephri:
Simplicity: A field around Kephri flattens a small bullet's trajectory, forcing it to move either horizontally or vertically.
Little Ladybug: After a brief pause, a field spawns a ladybug that chases the opponent.

I hope you enjoy my little game!
>Note: I own very few of these assets. The ingame character sprites (not the character portraits) are probably the only things I spent a lot of artistic time on. (Not creative, I know)
>Note: This game is conceptually inspired by Touhou 9: Phantasmagoria of Flower View.  It is a fantastic game and I strongly recommend it.
>Note: I had fun making this.  It was a good class.  =)

