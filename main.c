/*
MIT License

Copyright (c) 2020 Richard Marks

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*
  This program depends on the ezal library

  https://github.com/RichardMarks/ezal
*/
#include <ezal.h>

/*
  helper function to get a psuedo-random int >= low and < high
  a better random number generator than rand() would be ideal
  but for the sake of this example, it gets the job done.
*/
static int random_in_range(int low, int high)
{
  return low + rand() % (high - low);
}

/*
  data structure that represents a single ball object
  x and y represent the center coordinate of the ball

  x_speed and y_speed represent the horizontal and
  vertical movement respectively.
*/
struct Ball {
  float x;
  float y;
  float x_speed;
  float y_speed;
};

/*
  this example will display BALL_COUNT balls bouncing on the screen
  the BALL_RADIUS is hard-coded here, but should be obtained from
  the ball.png graphics file.
*/
#define BALL_COUNT 32
#define BALL_RADIUS 32

/*
  data structure that represents our entire example "game" data
  an array of ball objects and a bitmap to share since loading
  the same bitmap for all the balls would be a waste of memory.
*/
struct GameData {
  struct Ball balls[BALL_COUNT];
  ALLEGRO_BITMAP* ball_bitmap;
};

/*
  we are going to use a global pointer to the instance of the
  game data structure that we create in the main() function
  in order to access it from all of the functions of the example.
*/
struct GameData* game_data = 0;

/*
  function prototypes to write the function body after main()
  where they are referenced from.
*/
EZAL_FN(my_create_fn);
EZAL_FN(my_destroy_fn);
EZAL_FN(my_update_fn);
EZAL_FN(my_render_fn);

/*
  program entry point
  create the data structures and start the ezal runtime
*/
int main(int argc, char** argv)
{
  struct EZALConfig cfg;
  struct GameData game_data_obj;
  game_data = &game_data_obj;

  ezal_use_config_defaults(&cfg);

  srand(time(0));

  return ezal_start(
    "EZAL Bouncing Ball Example",
    &my_create_fn,
    &my_destroy_fn,
    &my_update_fn,
    &my_render_fn,
    &cfg);
}

void initialize_ball(struct Ball* ball, struct EZALRuntimeContext* ctx)
{
  // random position on screen
  int x = random_in_range(BALL_RADIUS, ctx->cfg->logical_width - BALL_RADIUS);
  int y = random_in_range(BALL_RADIUS, ctx->cfg->logical_height - BALL_RADIUS);

  // random movement
  int x_speed = random_in_range(-10, 10);
  int y_speed = random_in_range(-10, 10);

  // ensure no balls have a speed of zero
  x_speed = x_speed == 0 ? 1 : x_speed;
  y_speed = y_speed == 0 ? 1 : y_speed;

  // set the ball properties
  ball->x = x;
  ball->y = y;
  ball->x_speed = x_speed;
  ball->y_speed = y_speed;
}

void move_ball(struct Ball* ball)
{
  // add the ball speed to the ball position to make it move
  ball->x += ball->x_speed;
  ball->y += ball->y_speed;
}

void bounce_ball(struct Ball* ball, int left, int top, int right, int bottom)
{
  // if the left edge of the ball hits the left boundary
  // or the right edge of the ball hits the right boundary
  // move the ball in the opposite direction along the x axis
  // and reverse the horizontal movement speed
  if (ball->x - BALL_RADIUS < left || ball->x + BALL_RADIUS > right)
  {
    ball->x -= ball->x_speed;
    ball->x_speed = -ball->x_speed;
  }

  // if the top edge of the ball hits the top boundary
  // or the bottom edge of the ball hits the bottom boundary
  // move the ball in the opposite direction along the y axis
  // and reverse the vertical movement speed
  if (ball->y - BALL_RADIUS < top || ball->y + BALL_RADIUS > bottom)
  {
    ball->y -= ball->y_speed;
    ball->y_speed = -ball->y_speed;
  }
}

void draw_ball(struct Ball* ball)
{
  // draw the ball offset by the radius so that the x and y
  // represent the center of the ball and not the top-left corner
  al_draw_bitmap(
    game_data->ball_bitmap,
    ball->x - BALL_RADIUS,
    ball->y - BALL_RADIUS,
    0);
}

EZAL_FN(my_create_fn)
{
  // create things
  game_data->ball_bitmap = al_load_bitmap("ball.png");
  if (!game_data->ball_bitmap)
  {
    fprintf(stderr, "unable to load ball.png\n");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < BALL_COUNT; i++)
  {
    struct Ball* ball = &game_data->balls[i];
    initialize_ball(ball, ctx);
  }
}

EZAL_FN(my_destroy_fn)
{
  // destroy things
  if (game_data->ball_bitmap)
  {
    al_destroy_bitmap(game_data->ball_bitmap);
    game_data->ball_bitmap = 0;
  }
}

EZAL_FN(my_update_fn)
{
  // update things
  for (int i = 0; i < BALL_COUNT; i++)
  {
    struct Ball* ball = &game_data->balls[i];
    move_ball(ball);
    bounce_ball(ball, 0, 0, ctx->cfg->logical_width, ctx->cfg->logical_height);
  }

  if (EZAL_KEY(ALLEGRO_KEY_ESCAPE))
  {
    ezal_stop(ctx);
  }
}

EZAL_FN(my_render_fn)
{
  // draw things
  for (int i = 0; i < BALL_COUNT; i++)
  {
    struct Ball* ball = &game_data->balls[i];
    draw_ball(ball);
  }
}
