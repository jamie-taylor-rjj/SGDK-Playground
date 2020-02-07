#include <genesis.h>
#include <resources.h>
#include <string.h>

// Some game specific variables
int game_on = FALSE;
char msg_start[22] = "PRESS START TO BEGIN!\0";
char msg_reset[37] = "GAME OVER! PRESS START TO PLAY AGAIN.";

// Create a variable to store a sprite for the ball
Sprite* ball;
// Create a vaiable to store a sprite for the paddle/player
Sprite* player;

// Score variables
int score = 0;
char label_score[6] = "SCORE\0";
char str_score[3] = "0";

// screen edges
const int LEFT_EDGE = 0;
const int RIGHT_EDGE = 320;
const int TOP_EDGE = 0;
const int BOTTOM_EDGE = 224;

// Ball variables
int ball_pos_x = 100;
int ball_pos_y = 100;
int ball_vel_x = 1;
int ball_vel_y = 1;
int ball_width = 8;
int ball_height = 8;

// Paddle/Player variables
int player_pos_x = 144;
const int player_pos_y = 200;
int player_vel_x = 0;
const int player_width = 32;
const int player_height = 8;

// Helper method which will swap the sign of a value which is passed to it
int sign(int x) {
    return (x > 0) - (x < 0);
}

// A helper function which draws text in the center of the screen
void showText(char s[]){
	VDP_drawText(s, 20 - strlen(s)/2 ,15);
}

// Stops the game when we hit the game over state
void endGame(){
	showText(msg_reset);
	game_on = FALSE;
}

void updateScoreDisplay(){
	sprintf(str_score,"%d",score);
	VDP_clearText(1,2,3);
	VDP_drawText(str_score,1,2);
}

// Used to start the game
void startGame(){
	score = 0;
	updateScoreDisplay();

	ball_pos_x = 0;
	ball_pos_y = 0;

	ball_vel_x = 1;
	ball_vel_y = 1;

	// Clear the text from the screen
	VDP_clearTextArea(0,10,40,10);

	game_on = TRUE;
}

void moveBall(){
	//Check horizontal bounds
	if(ball_pos_x < LEFT_EDGE){
		ball_pos_x = LEFT_EDGE;
		ball_vel_x = -ball_vel_x;
	} else if(ball_pos_x + ball_width > RIGHT_EDGE){
		ball_pos_x = RIGHT_EDGE - ball_width;
		ball_vel_x = -ball_vel_x;
	}

	//Check vertical bounds
	if(ball_pos_y < TOP_EDGE){
		ball_pos_y = TOP_EDGE;
		ball_vel_y = -ball_vel_y;
	} else if(ball_pos_y + ball_height > BOTTOM_EDGE){
		endGame();
	}

	//Check for collision with paddle
	if(ball_pos_x < player_pos_x + player_width && ball_pos_x + ball_width > player_pos_x){
		if(ball_pos_y < player_pos_y + player_height && ball_pos_y + ball_height >= player_pos_y){
			//On collision, invert the velocity
			ball_pos_y = player_pos_y - ball_height - 1;
			ball_vel_y = -ball_vel_y;

			//Increase the score and update the HUD
			score++;
			updateScoreDisplay();

			//Make ball faster on every 10th hit
			if( score % 10 == 0){
				ball_vel_x += sign(ball_vel_x);
				ball_vel_y += sign(ball_vel_y);
			}
		}
	}

	//Position the ball
	ball_pos_x += ball_vel_x;
	ball_pos_y += ball_vel_y;

	SPR_setPosition(ball,ball_pos_x,ball_pos_y);	
}

void positionPlayer(){
	// Add the player's velocity to its position
	player_pos_x += player_vel_x;

	// Keep the player within the bounds of the screen
	if(player_pos_x < LEFT_EDGE) player_pos_x = LEFT_EDGE;
	if(player_pos_x + player_width > RIGHT_EDGE) player_pos_x = RIGHT_EDGE - player_width;

	// Let the Sprite engine position the sprite
	SPR_setPosition(player,player_pos_x,player_pos_y);
}

void myJoyHandler(u16 joy, u16 changed, u16 state)
{
	if (joy == JOY_1)
	{
		// Ensure that the game starts when we hit the start button
		if(state & BUTTON_START){
			if(!game_on){
				startGame();
			}
		}

		// Set player velocity if left or right are pressed
		// Set velocity to 0 if no direction is pressed
		if (state & BUTTON_RIGHT)
		{
			player_vel_x = 3;
		}
		else if (state & BUTTON_LEFT)
		{
			player_vel_x = -3;
		} else{
			if( (changed & BUTTON_RIGHT) | (changed & BUTTON_LEFT) ){
				player_vel_x = 0;
			}
		}
	}
}

int main()
{
	// Initialise the controller stack
	JOY_init();
	// Set the controller callback
	JOY_setEventHandler( &myJoyHandler );
	// Both of the above lines need to be included (in that order) in order to
	// take in the input from controllers.
	// Without the initialisation call, we won't be able to assign the event handler

	// Load textures
	VDP_loadTileSet(bgtile.tileset,1,DMA);

	// Ensure the pallet is loaded, otherwise the tile will be displayed using
	//the default (i.e. white)
	VDP_setPalette(PAL1, bgtile.palette->data);

	// Initialise the sprites engine
	SPR_init(0,0,0);

	// load the ball sprite into the ball variable
	// NOTE: the coordinates for x and y here (100, 100) are given in pixels NOT
	// tiles - because that makes sense
	ball = SPR_addSprite(&imgball,100,100,TILE_ATTR(PAL1,0, FALSE, FALSE));
	
	// load the paddle/player sprite into the player variable
	player = SPR_addSprite(&paddle, player_pos_x, player_pos_y, TILE_ATTR(PAL1, 0, FALSE, FALSE));

	// Fill the screen with the tile, using the pallet data
	VDP_fillTileMapRect(PLAN_B,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,1),0,0,40,30);

	// Draw the texts - ensure that they are drawn on the foreground (i.e. Plane A)
	VDP_setTextPlan(PLAN_B);
	VDP_drawText(label_score,1,1);
	// Ensure that the score is displayed as soon as possible
	updateScoreDisplay();
	// Ensure that a prompt to start the game is displayed
	showText(msg_start);

	while(1)
	{
		// Only move things IFF the game is running
		if(game_on == TRUE){
			// Move the ball!
			moveBall();
			// Update the player position
			positionPlayer();
		}

		// Draw sprites wherever they should appear
		SPR_update();
		// Ensure that we v-sync before running the game loop again
		VDP_waitVSync();
	}

	return(0);
}

