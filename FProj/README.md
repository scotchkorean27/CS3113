#I made a game!

#Creative Title (I'm standing by that name) 
is a split screen side by side bullet hell shooter.</br>
The goal is to send bullets over to your opponent's side of the screen and overwhelm them until they run out of lives.</br>
Note: to play this game, you need a controller.</br>
I used a PS3 controller with the MotionInJoy drivers.</br>
Other controllers should work, but you need to remap them.</br>
The mapping can be found in Game.cpp in the #define section</br>


##Controls:
###Keyboard/Player 1:
Z: Select/Player 1 shoot</br>
X (Keyboard)/O(PS3 Controller): Cancel/Back/Charge Super</br>
Directional Keys: Player 1 Move</br>
Left-Shift: Player 1 Focus Movement + Defensive Super</br>


###Controller/Player 2:
X (PS3 Controller): Select/Player 2 shoot</br>
PS3 D-Pad: Player 2 Move</br>
R1 (PS3 Controller): Player 2 Focus Movement + Defensive Special</br>
L1(PS3 Controlelr): Player 2 Charge Super</br>
O (PS3 Controller): Cancel/Back</br>

##Basic Gameplay:
Hold your shot button for continuous fire, and try to hit the enemies!</br>
When you kill an enemy, they will explode, destroying all small bullets in a small area.</br>
A small handful of bullets plus any destroyed bullets will appear at the top of your opponent's screen.</br>
Enemy spawn and firing rates will increase as the game goes on, so don't dilly-dally!</br>
When you get hit, you will be invincible for a while.</br>
After that wears off, you will destroy all bullets and offensive skills in a large area.</br>

##Focused Movement:
Holding your focus button (Left-Shift or R1) will invoke focused movement.</br>
During focused movement, you move slower, allowing for more precise adjustments.</br>
Additionally, a small white square will appear on your character.  That is your hitbox.</br>
Additionally, focused movement will activate your character's defensive special.</br>
The defensive special will deactivate once the button is released.</br>

##Scoring:
Points are obtained by killing enemies or destroying bullets (by means other than getting hit).</br>
A small bonus multiplier is applied by getting points often in a short amount of time.</br>
Therefore, a good point gathering strategy is to lets enemies build up, and firing away to land continuous kills.</br>
After you get enough points, you will send over your character's offensive special!</br>
Different characters have different point requirements.</br>
Stronger offensives require much higher scoring combos.</br>
Scoring points also fills up your bar, allowing you to use supers!</br>

##Supers:
At the bottom of your screen is a small gauge that fills up as you score.</br>
A super has three levels, corresponding to the three sections in the gauge.</br>
Holding your charge button (X in keyboard, O in controller) will cause you to charge your super.</br>
Your gauge will fill with orange and you will hear a small sound when your charge reaches certain levels.</br>
When you are satisfied with your charge, release the charge button to use your super.</br>
The longer you charge your super, the more powerful and expensive it will be.</br>
Charging your super takes a while, so know when you need it in advance.</br>
A super can have up to three levels:</br></br>
Level 1: Destroys all small bullets in a small area.  Grants brief invincibility.</br>
Level 2: Destroys all bullets in a medium area.  Grants invincibility.</br>
Level 3: Destroys all bullets and supers in a massive area.  Grants prolonged invincibility.</br></br>
All bullets destroyed by your super contribute to your points, and therefore your offensive specials.</br>
However, points obtained this way will NOT contribute to your charge (that would be circular and overpowered).</br>

##Apocalypse:
While the first 6 stages are mechanically similar, Apocalypse is a special seventh stage.</br>
Both players begin with 20 lives, instead of 5.</br>
At the beginning, a boss will spawn on both sides of the screen.</br>
Hitting the boss will grant points and send bullets to your opponent's side of the screen.</br>
The boss will regularly spawn regular enemies that must also be dealt with.</br>
Additionally, the boss will regularly attack with offensive specials.</br>
The boss can spawn different specials at the same time, and some combinations are especially deadly together!</br>
The attack patterns will get increasingly difficult over time.</br>
During apocalypse, a gauge will appear at the top of the screen.  It will fill up over time.</br>
WHen it's full, the real challenge starts.</br>
You can work together to survive, or try to push your opponent into oblivion.</br>
Do your best to survive the end of the world!</br>

##Characters:
Each character has a unique offensive and defensive skill.

###Corvus:
Time Crunch: A field around Corvus slows down all nearby bullets.</br>
Time Dilation: A magic clock on the field temporarily speeds up small bullets within the circle.</br>

###Ariel:
Holy Orb: A slow but powerful shot that plows through enemies.  Great for clearing crowded areas.</br>
Cross Fire: A cross appears on the screen that periodically shoots bullets in four directions.</br>

###Flora:
Flower Power: Fires bullets in eight directions at a slightly decreased firing rate.</br>
Blooming Bullet: A flower blooms on the opponent's screen.  After a pause, it shoots bullets in 6 directions.</br>

###Thor:
Dominion: Thor is immobile while focused, and movement will move a targeting reticle.  Release the focus button to teleport Thor to the targeting reticle.</br>
Judgment: A magic field that redirects nearby large bullets toward the opponent.</br>

###Crystal:
Crystal Cutter: A small 3D crystal revolves around Crystal, destroying any small bullets it comes into contact with.</br>
Crystal Clear: Large gemstones obscure the opponent's field.  Hiding bullets, enemeies, and other offensive specials.</br>

###Kephri:
Simplicity: A field around Kephri flattens a small bullet's trajectory, forcing it to move either horizontally or vertically.</br>
Little Ladybug: After a brief pause, a field spawns a ladybug that chases the opponent.</br>

####I hope you enjoy my little game!</br>
>Note: I own very few of these assets. The ingame character sprites (not the character portraits) are probably the only</br> things I spent a lot of artistic time on. (Not creative, I know)</br>
>Note: This game is conceptually inspired by Touhou 9: Phantasmagoria of Flower View.  It is a fantastic game and I strongly recommend it.</br>
>Note: I had fun making this.  It was a good class.  =)

