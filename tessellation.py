
#-----Statement of Authorship----------------------------------------#
#
#  This is an individual assessment item.  By submitting this
#  code I agree that it represents my own work.  I am aware of
#  the University rule that a student must not act in a manner
#  which constitutes academic dishonesty as stated and explained
#  in QUT's Manual of Policies and Procedures, Section C/5.3
#  "Academic Integrity" and Section E/2.1 "Student Code of Conduct".
#
#    Student no: n10535268
#    Student name: Joseph Haddad
#
#  NB: Files submitted without a completed copy of this statement
#  will not be marked.  All files submitted will be subjected to
#  software plagiarism analysis using the MoSS system
#  (http://theory.stanford.edu/~aiken/moss/).
#
#--------------------------------------------------------------------#



#-----Task Description-----------------------------------------------#
#
#  TESSELLATION
#
#  This assignment tests your skills at processing data stored in
#  lists, creating reusable code and following instructions to display
#  a complex visual image.  The incomplete Python program below is
#  missing a crucial function, "tessellate".  You are required to
#  complete this function so that when the program is run it fills
#  a rectangular space with differently-shaped tiles, using data
#  stored in a list to determine which tiles to place and where.
#  See the instruction sheet accompanying this file for full details.
#
#  Note that this assignment is in two parts, the second of which
#  will be released only just before the final deadline.  This
#  template file will be used for both parts and you will submit
#  your final solution as a single Python 3 file, whether or not you
#  complete both parts of the assignment.
#
#--------------------------------------------------------------------#  



#-----Preamble-------------------------------------------------------#
#
# This section imports necessary functions and defines constant
# values used for creating the drawing canvas.  You should not change
# any of the code in this section.
#

# Import the functions needed to complete this assignment.  You
# should not need to use any other modules for your solution.  In
# particular, your solution must not rely on any non-standard Python
# modules that need to be downloaded and installed separately,
# because the markers will not have access to such modules.

from turtle import *
from math import *
from random import *

# Define constant values used in the main program that sets up
# the drawing canvas.  Do not change any of these values.

cell_size = 100 # pixels (default is 100)
grid_width = 10 # squares (default is 10)
grid_height = 7 # squares (default is 7)
x_margin = cell_size * 2.75 # pixels, the size of the margin left/right of the grid
y_margin = cell_size // 2 # pixels, the size of the margin below/above the grid
window_height = grid_height * cell_size + y_margin * 2
window_width = grid_width * cell_size + x_margin * 2
small_font = ('Arial', 18, 'normal') # font for the coords
big_font = ('Arial', 24, 'normal') # font for any other text

# Validity checks on grid size - do not change this code
assert cell_size >= 80, 'Cells must be at least 80x80 pixels in size'
assert grid_width >= 8, 'Grid must be at least 8 squares wide'
assert grid_height >= 6, 'Grid must be at least 6 squares high'

#
#--------------------------------------------------------------------#



#-----Functions for Creating the Drawing Canvas----------------------#
#
# The functions in this section are called by the main program to
# manage the drawing canvas for your image.  You should not change
# any of the code in this section.
#

# Set up the canvas and draw the background for the overall image
def create_drawing_canvas(bg_colour = 'light blue',
                          line_colour = 'slate grey',
                          draw_grid = True, mark_legend = False):
    
    # Set up the drawing canvas with enough space for the grid and
    # legend
    setup(window_width, window_height)
    bgcolor(bg_colour)

    # Draw as quickly as possible
    tracer(False)

    # Get ready to draw the grid
    penup()
    color(line_colour)
    width(2)

    # Determine the left-bottom coords of the grid
    left_edge = -(grid_width * cell_size) // 2 
    bottom_edge = -(grid_height * cell_size) // 2

    # Optionally draw the grid
    if draw_grid:

        # Draw the horizontal grid lines
        setheading(0) # face east
        for line_no in range(0, grid_height + 1):
            penup()
            goto(left_edge, bottom_edge + line_no * cell_size)
            pendown()
            forward(grid_width * cell_size)
            
        # Draw the vertical grid lines
        setheading(90) # face north
        for line_no in range(0, grid_width + 1):
            penup()
            goto(left_edge + line_no * cell_size, bottom_edge)
            pendown()
            forward(grid_height * cell_size)

        # Draw each of the labels on the x axis
        penup()
        y_offset = 27 # pixels
        for x_label in range(0, grid_width):
            goto(left_edge + (x_label * cell_size) + (cell_size // 2), bottom_edge - y_offset)
            write(chr(x_label + ord('A')), align = 'center', font = small_font)

        # Draw each of the labels on the y axis
        penup()
        x_offset, y_offset = 7, 10 # pixels
        for y_label in range(0, grid_height):
            goto(left_edge - x_offset, bottom_edge + (y_label * cell_size) + (cell_size // 2) - y_offset)
            write(str(y_label + 1), align = 'right', font = small_font)

        # Mark centre coordinate (0, 0)
        home()
        dot(15)

    # Optionally mark the spaces for drawing the legend
    if mark_legend:
        # Left side
        goto(-(grid_width * cell_size) // 2 - 75, -25)
        write('Put your\nlegend here', align = 'right', font = big_font)    
        # Right side
        goto((grid_width * cell_size) // 2 + 75, -25)
        write('Put your\nlegend here', align = 'left', font = big_font)    

    # Reset everything ready for the student's solution
    pencolor('black')
    width(1)
    penup()
    home()
    tracer(True)


# End the program and release the drawing canvas to the operating
# system.  By default the cursor (turtle) is hidden when the
# program ends - call the function with False as the argument to
# prevent this.
def release_drawing_canvas(hide_cursor = True):
    tracer(True) # ensure any drawing still in progress is displayed
    if hide_cursor:
        hideturtle()
    done()
    
#
#--------------------------------------------------------------------#



#-----Test Data for Use During Code Development----------------------#
#
# The "fixed" data sets in this section are provided to help you
# develop and test your code.  You can use them as the argument to
# the "tesselate" function while perfecting your solution.  However,
# they will NOT be used to assess your program.  Your solution will
# be assessed using the "random_pattern" function appearing below.
# Your program must work correctly for any data set that can be
# generated by the random_pattern function.
#
# Each of the data sets is a list of instructions, each specifying
# where to place a particular tile.  The general form of each
# instruction is
#
#     [squares, mystery_value]
#
# where there may be one, two or four squares in the grid listed
# at the beginning.  This tells us which grid squares must be
# filled by this particular tile.  This information also tells
# us which shape of tile to produce.  A "big" tile will occupy
# four grid squares, a "small" tile will occupy one square, a
# "wide" tile will occupy two squares in the same row, and a
# "tall" tile will occupy two squares in the same column.  The
# purpose of the "mystery value" will be revealed in Part B of
# the assignment.
#
# Note that the fixed patterns below assume the grid has its
# default size of 10x7 squares.
#

# Some starting points - the following fixed patterns place
# just a single tile in the grid, in one of the corners.

# Small tile
fixed_pattern_0 = [['A1', 'O']] 
fixed_pattern_1 = [['J7', 'X']]

# Wide tile
fixed_pattern_2 = [['A7', 'B7', 'O']] 
fixed_pattern_3 = [['I1', 'J1', 'X']]

# Tall tile
fixed_pattern_4 = [['A1', 'A2', 'O']] 
fixed_pattern_5 = [['J6', 'J7', 'X']]

# Big tile
fixed_pattern_6 = [['A6', 'B6', 'A7', 'B7', 'O']] 
fixed_pattern_7 = [['I1', 'J1', 'I2', 'J2', 'X']]

# Each of these patterns puts multiple copies of the same
# type of tile in the grid.

# Small tiles
fixed_pattern_8 = [['E1', 'O'],
                   ['J4', 'O'],
                   ['C5', 'O'],
                   ['B1', 'O'],
                   ['I1', 'O']] 
fixed_pattern_9 = [['C6', 'X'],
                   ['I4', 'X'],
                   ['D6', 'X'],
                   ['J5', 'X'],
                   ['F6', 'X'],
                   ['F7', 'X']]

# Wide tiles
fixed_pattern_10 = [['A4', 'B4', 'O'],
                    ['C1', 'D1', 'O'],
                    ['C7', 'D7', 'O'],
                    ['A7', 'B7', 'O'],
                    ['D4', 'E4', 'O']] 
fixed_pattern_11 = [['D7', 'E7', 'X'],
                    ['G7', 'H7', 'X'],
                    ['H5', 'I5', 'X'],
                    ['B3', 'C3', 'X']]

# Tall tiles
fixed_pattern_12 = [['J2', 'J3', 'O'],
                    ['E5', 'E6', 'O'],
                    ['I1', 'I2', 'O'],
                    ['E1', 'E2', 'O'],
                    ['D3', 'D4', 'O']] 
fixed_pattern_13 = [['H4', 'H5', 'X'],
                    ['F1', 'F2', 'X'],
                    ['E2', 'E3', 'X'],
                    ['C4', 'C5', 'X']]

# Big tiles
fixed_pattern_14 = [['E5', 'F5', 'E6', 'F6', 'O'],
                    ['I5', 'J5', 'I6', 'J6', 'O'],
                    ['C2', 'D2', 'C3', 'D3', 'O'],
                    ['H2', 'I2', 'H3', 'I3', 'O'],
                    ['A3', 'B3', 'A4', 'B4', 'O']] 
fixed_pattern_15 = [['G2', 'H2', 'G3', 'H3', 'X'],
                    ['E5', 'F5', 'E6', 'F6', 'X'],
                    ['E3', 'F3', 'E4', 'F4', 'X'],
                    ['B3', 'C3', 'B4', 'C4', 'X']]

# Each of these patterns puts one instance of each type
# of tile in the grid.
fixed_pattern_16 = [['I5', 'O'],
                    ['E1', 'F1', 'E2', 'F2', 'O'],
                    ['J5', 'J6', 'O'],
                    ['G7', 'H7', 'O']]
fixed_pattern_17 = [['G7', 'H7', 'X'],
                    ['B7', 'X'],
                    ['A5', 'B5', 'A6', 'B6', 'X'],
                    ['D2', 'D3', 'X']]

# If you want to create your own test data sets put them here,
# otherwise call function random_pattern to obtain data sets
# that fill the entire grid with tiles.
 
#
#--------------------------------------------------------------------#



#-----Function for Assessing Your Solution---------------------------#
#
# The function in this section will be used to assess your solution.
# Do not change any of the code in this section.
#
# The following function creates a random data set specifying a
# tessellation to draw.  Your program must work for any data set that
# can be returned by this function.  The results returned by calling
# this function will be used as the argument to your "tessellate"
# function during marking.  For convenience during code development
# and marking this function also prints the pattern to be drawn to the
# shell window.  NB: Your solution should not print anything else to
# the shell.  Make sure any debugging calls to the "print" function
# are disabled before you submit your solution.
#
# This function attempts to place tiles using a largest-to-smallest
# greedy algorithm.  However, it randomises the placement of the
# tiles and makes no attempt to avoid trying the same location more
# than once, so it's not very efficient and doesn't maximise the
# number of larger tiles placed.  In the worst case, only one big
# tile will be placed in the grid (but this is very unlikely)!
#
# As well as the coordinates for each tile, an additional value which
# is either an 'O' or 'X' accompanies each one.  The purpose of this
# "mystery" value will be revealed in Part B of the assignment.
#
def random_pattern(print_pattern = True):
    # Keep track of squares already occupied
    been_there = []
    # Initialise the pattern
    pattern = []
    # Percent chance of the mystery value being an X
    mystery_probability = 8

    # Attempt to place as many 2x2 tiles as possible, up to a fixed limit
    attempts = 10
    while attempts > 0:
        # Choose a random bottom-left location
        column = randint(0, grid_width - 2)
        row = randint(0, grid_height - 2)
        # Try to place the tile there, provided the spaces are all free
        if (not [column, row] in been_there) and \
           (not [column, row + 1] in been_there) and \
           (not [column + 1, row] in been_there) and \
           (not [column + 1, row + 1] in been_there):
            been_there = been_there + [[column, row], [column, row + 1],
                                       [column + 1, row], [column + 1, row + 1]]
            # Append the tile's coords to the pattern, plus the mystery value
            pattern.append([chr(column + ord('A')) + str(row + 1),
                            chr(column + ord('A') + 1) + str(row + 1),
                            chr(column + ord('A')) + str(row + 2),
                            chr(column + ord('A') + 1) + str(row + 2),
                            'X' if randint(1, 100) <= mystery_probability else 'O'])
        # Keep track of the number of attempts
        attempts = attempts - 1

    # Attempt to place as many 1x2 tiles as possible, up to a fixed limit
    attempts = 15
    while attempts > 0:
        # Choose a random bottom-left location
        column = randint(0, grid_width - 1)
        row = randint(0, grid_height - 2)
        # Try to place the tile there, provided the spaces are both free
        if (not [column, row] in been_there) and \
           (not [column, row + 1] in been_there):
            been_there = been_there + [[column, row], [column, row + 1]]
            # Append the tile's coords to the pattern, plus the mystery value
            pattern.append([chr(column + ord('A')) + str(row + 1),
                            chr(column + ord('A')) + str(row + 2),
                            'X' if randint(1, 100) <= mystery_probability else 'O'])
        # Keep track of the number of attempts
        attempts = attempts - 1
        
    # Attempt to place as many 2x1 tiles as possible, up to a fixed limit
    attempts = 20
    while attempts > 0:
        # Choose a random bottom-left location
        column = randint(0, grid_width - 2)
        row = randint(0, grid_height - 1)
        # Try to place the tile there, provided the spaces are both free
        if (not [column, row] in been_there) and \
           (not [column + 1, row] in been_there):
            been_there = been_there + [[column, row], [column + 1, row]]
            # Append the tile's coords to the pattern, plus the mystery value
            pattern.append([chr(column + ord('A')) + str(row + 1),
                            chr(column + ord('A') + 1) + str(row + 1),
                            'X' if randint(1, 100) <= mystery_probability else 'O'])
        # Keep track of the number of attempts
        attempts = attempts - 1
        
    # Fill all remaining spaces with 1x1 tiles
    for column in range(0, grid_width):
        for row in range(0, grid_height):
            if not [column, row] in been_there:
                been_there.append([column, row])
                # Append the tile's coords to the pattern, plus the mystery value
                pattern.append([chr(column + ord('A')) + str(row + 1),
                                'X' if randint(1, 100) <= mystery_probability else 'O'])

    # Remove any residual structure in the pattern
    shuffle(pattern)
    # Print the pattern to the shell window, nicely laid out
    print('Draw the tiles in this sequence:')
    print(str(pattern).replace('],', '],\n'))
    # Return the tessellation pattern
    return pattern

#
#--------------------------------------------------------------------#



#-----Student's Solution---------------------------------------------#
#
#  Complete the assignment by replacing the dummy function below with
#  your own "tessellate" function.
#


def two_by_two(): ### Red

    ### Draw the outer rectangle
    setheading(0)
    width(3)
    fillcolor('steel blue')
    begin_fill()
    pendown()
    forward(200)
    left(90)
    forward(200)
    left(90)
    forward(200)
    left(90)
    forward(200)
    end_fill()
    penup()

    ### Outer black circle
    setheading(0)
    forward(100)
    left(90)
    forward(100)
    pencolor('black')
    dot(170)
    left(180)
    forward(100)
    right(90)
    forward(100)
    setheading(270)

    # Positioning of the body
    forward(-120)
    right(90)
    forward(-60)
    left(90)

    ### Main body of bird
    width(5)
    fillcolor('Firebrick')
    begin_fill()
    pendown()
    right(45)
    circle(60, extent = 270)
    right(45)
    circle(50, extent = 70)
    left(30)
    circle(50, extent = 10 )
    left(30)
    circle(50, extent = 10 )
    left(30)
    circle(50, extent = 40 )
    right(190)
    circle(50, extent = 60 )
    left(30)
    circle(50, extent = 20 )
    left(30)
    circle(50, extent = 20 )
    left(70)
    forward(10)
    end_fill()
    penup()
    width(3)
    
    ### Positioning
    forward(70)

    ### Eye brows
    fillcolor('black')
    begin_fill()
    pendown()
    right(140)
    forward(30)
    right(60)
    forward(30)
    left(80)
    forward(10)
    left(105)
    forward(35)
    left(50)
    forward(35)
    left(80)
    forward(10)
    end_fill()
    penup()

    ### Positining
    forward(3)
    right(90)
    forward(-56)
    left(90)

    ### Left eye
    fillcolor('white')
    begin_fill()
    pendown()
    left(100)
    circle(20, extent = 30)
    left(30)
    circle(20, extent = 40)
    left(30)
    circle(20, extent = 80)
    left(30)
    forward(2)
    end_fill()
    penup()

    ### Positioning
    right(90)
    forward(10)
    left(90)

    ### Right eye
    pendown()
    fillcolor('white')
    begin_fill()
    pendown()
    left(175)
    circle(20, extent = 30)
    left(30)
    circle(20, extent = 40)
    left(30)
    circle(20, extent = 80)
    left(30)
    forward(2)
    end_fill()
    penup()

    ### Positioining
    setheading(180)
    forward(9)
    left(90)
    forward(15)

    #### Right eye dot
    pendown()
    dot(10)
    penup()

    ### Positioining
    setheading(180)
    forward(35)
    left(90)
    forward(-4)

    ### Left eye dot
    pendown()
    dot(10)
    penup()

    ### Positioning
    setheading(270)
    forward(50)
    setheading(0)
    forward(57)
    setheading(90)

    ### White belly section
    fillcolor('wheat')
    begin_fill()
    pendown()
    left(50)
    circle(50, extent = 94)
    setheading(0)
    right(38)
    circle(50, extent = 94)
    end_fill()
    penup()
    
    ### Positioning
    left(90)
    forward(70)
    right(90)
    right(30)
    forward(18)
    left(30)
    
    ### Beak
    width(3)
    fillcolor('Orange')
    begin_fill()
    pendown()
    left(170)
    circle(50, extent = 30)
    left(120)
    forward(30)
    left(120)
    forward(25)
    end_fill()
    penup()

    ### Positioning
    setheading(270)
    forward(22)

    ### Bottom part of beak
    width(2)
    fillcolor('Dark Orange')
    begin_fill()
    pendown()
    left(45)
    forward(10)
    left(100)
    forward(12)
    left(135)
    forward(12)
    end_fill()
    penup()

def one_by_one(): ### Yellow

    ### Tile outline
    setheading(0)
    pendown()
    setheading(0)
    width(3)
    pencolor('black')
    fillcolor('beige')
    begin_fill()
    forward(100)
    left(90)
    forward(100)
    left(90)
    forward(100)
    left(90)
    forward(100)
    end_fill()
    penup()

    ### Positioning
    setheading(0)
    forward(20)
    left(90)
    forward(10)

    ### Chucks body
    width(3)
    fillcolor('yellow')
    begin_fill()
    pendown()
    right(100)
    circle(200, extent = 20)
    left(90)
    circle(200, extent = 20)
    left(100)
    circle(200, extent = 24)
    left(120)
    forward(5)
    end_fill()

    ### Positioning
    penup()
    setheading(0)
    forward(50)
    left(90)
    forward(60)

    ### Spiky Hair
    fillcolor('black')
    begin_fill()
    pendown()
    right(50)
    circle(20, extent = 70)
    right(220)
    forward(10)
    right(110)
    circle(30, extent = 40)
    left(120)
    forward(15)
    right(150)
    circle(30, extent = 40)
    left(130)
    forward(20)
    end_fill()

    ### Positioning
    penup()
    setheading(0)
    right(90)
    forward(25)
    right(90)
    forward(5)

    ### Eyes
    penup()
    right(90)
    forward(-8)
    pendown()
    pencolor('black')
    dot(15)
    pencolor('white')
    dot(10)
    pencolor('black')
    dot(5)
    penup()
    right(90)
    forward(20)
    pendown()
    pencolor('black')
    dot(15)
    pencolor('white')
    dot(10)
    pencolor('black')
    dot(5)
    penup()

    ### Positioning
    left(180)
    forward(20)
    left(90)
    forward(12)
    left(100)

    ### Beak
    fillcolor('orange')
    begin_fill()
    pendown()
    width(2)
    forward(15)
    right(20)
    forward(15)
    setheading(180)
    left(10)
    forward(10)
    left(90)
    forward(10)
    left(10)
    forward(10)
    setheading(180)
    right(40)
    forward(32)
    end_fill()

    ### Positioning
    penup()
    setheading(180)
    forward(9)
    setheading(90)
    forward(25)

    ### Left eye brow
    pendown()
    pencolor('firebrick')
    fillcolor('firebrick')
    begin_fill()
    setheading(0)
    forward(20)
    left(90)
    forward(2)
    left(90)
    forward(20)
    left(90)
    forward(2)
    end_fill()

    ### Positioning
    penup()
    setheading(0)
    forward(20)
    setheading(90)
    forward(5)
    setheading(0)


    ### Right eye brow
    pendown()
    pencolor('firebrick')
    fillcolor('firebrick')
    begin_fill()
    setheading(0)
    forward(20)
    left(90)
    forward(2)
    left(90)
    forward(20)
    left(90)
    forward(2)
    end_fill()
    penup()

def horizontal(): ### Bad Piggie

    ### Tile outline
    setheading(0)
    fillcolor('sandy brown')
    begin_fill()
    width(3)
    pendown()
    forward(200)
    left(90)
    forward(100)
    left(90)
    forward(200)
    left(90)
    forward(100)
    end_fill()
    penup()

    ### Positioning
    setheading(0)
    forward(100)
    setheading(90)
    forward(5)

    ### Body
    pencolor('dark green')
    fillcolor('lawn green')
    begin_fill()
    pendown()
    setheading(0)
    circle(40)
    end_fill()

    ### Ears
    penup()
    circle(40, extent = 130)
    pendown()
    fillcolor('lawn green')
    begin_fill()
    circle(-10)
    end_fill()
    penup()
    circle(40, extent = 90)
    pendown()
    fillcolor('lawn green')
    begin_fill()
    circle(-10)
    end_fill()
    penup()
    circle(40, extent = 150)
    setheading(90)
    forward(40)


    ### Eyes
    setheading(180)
    forward(30)
    pendown()
    pencolor('green')
    dot(20)
    pencolor('white')
    dot(15)
    pencolor('black')
    dot(5)
    penup()
    setheading(0)
    forward(25)
    pendown()
    pencolor('green')
    dot(30)
    pencolor('yellow green')
    dot(25)

    ### Nose holes
    penup()
    setheading(180)
    forward(5)
    pendown()
    pencolor('dark green')
    dot(10)
    setheading(0)
    penup()
    forward(10)
    pendown()
    dot(10)
    penup()
    forward(20)
    pendown()
    dot(20)
    pencolor('white')
    dot(15)
    pencolor('black')
    dot(5)

    ### Positioning
    pencolor('dark green')
    penup()
    setheading(180)
    forward(25)
    setheading(270)
    forward(20)
    dot(15)
    penup()
    forward(-5)
    pencolor('white')
    pendown()
    dot(5)
    setheading(0)
    forward(-5)
    dot(5)
    forward(10)
    dot(5)
    penup()

    ### TNT BOX
    setheading(180)
    forward(100)
    setheading(270)
    pencolor('saddle brown')
    fillcolor('burlywood')
    begin_fill()
    pendown()
    forward(25)
    left(90)
    forward(25)
    left(90)
    forward(25)
    left(90)
    forward(25)
    penup()
    left(90)
    forward(15)
    left(90)
    forward(5)
    end_fill()
    pencolor('red')
    write('TNT')

    ### TNT BOX
    penup()
    forward(15)
    left(90)
    forward(60)
    setheading(270)
    pencolor('saddle brown')
    fillcolor('burlywood')
    begin_fill()
    pendown()
    forward(25)
    left(90)
    forward(25)
    left(90)
    forward(25)
    left(90)
    forward(25)
    penup()
    left(90)
    forward(15)
    left(90)
    forward(5)
    end_fill()
    pencolor('red')
    write('TNT')

    ### TNT BOX
    penup()
    forward(120)
    right(90)
    forward(30)
    pencolor('saddle brown')
    fillcolor('burlywood')
    begin_fill()
    pendown()
    forward(25)
    left(90)
    forward(25)
    left(90)
    forward(25)
    left(90)
    forward(25)
    penup()
    left(90)
    forward(15)
    left(90)
    forward(5)
    end_fill()
    pencolor('red')
    write('TNT')

def vertical(): ### The Blues

    ### Tile outline
    setheading(0)
    fillcolor('light coral')
    begin_fill()
    width(3)
    pendown()
    forward(100)
    left(90)
    forward(200)
    left(90)
    forward(100)
    left(90)
    forward(200)
    end_fill()
    penup()

    ### Positioning
    setheading(0)
    forward(30)
    left(90)
    forward(30)

    ### Body
    fillcolor('steel blue')
    begin_fill()
    pendown()
    setheading(0)
    right(19)
    circle(50, extent = 110)
    setheading(90)
    forward(40)
    circle(50, extent = 50)
    right(40)
    circle(20, extent = 90)
    left(30)
    circle(20, extent = 20)
    left(30)
    circle(20, extent = 20)
    left(40)
    forward(15)
    right(180)
    forward(15)
    circle(50, extent = 20)
    left(30)
    circle(20, extent = 30)
    left(30)
    circle(20, extent = 30)
    left(50)
    forward(10)
    right(121)
    circle(80, extent = 94)
    end_fill()

    ### Positioning
    penup()
    setheading(90)
    forward(90)
    setheading(0)
    forward(10)

    ### Left eye
    pendown()
    dot(30)
    pencolor('white')
    dot(25)
    penup()
    forward(7)
    pendown()
    pencolor('black')
    dot(10)

    ### Positioning
    penup()
    forward(30)

    ### Right eye
    pendown()
    dot(30)
    pencolor('white')
    dot(25)
    penup()
    forward(7)
    pendown()
    pencolor('black')
    dot(10)

    ### Positioning
    penup()
    setheading(270)
    forward(15)
    setheading(180)
    forward(45)

    ### Orange bit under beak
    fillcolor('firebrick')
    begin_fill()
    pendown()
    left(130)
    circle(50, extent = 60)
    left(90)
    forward(18)
    left(80)
    forward(40)
    end_fill()

    ### Positioning
    penup()
    left(55)
    forward(20)
    left(170)

    ### Beak
    fillcolor('orange')
    begin_fill()
    pendown()
    forward(30)
    right(75)
    forward(45)
    setheading(180)
    forward(60)
    end_fill()
    fillcolor('orange')
    begin_fill()
    setheading(270)
    left(30)
    forward(20)
    left(80)
    forward(42)
    end_fill()
    penup()

def broken_tile_two_by_two(): ### Broken Red
    setheading(0)
    width(3)
    fillcolor('steel blue')
    begin_fill()
    pendown()
    forward(200)
    left(90)
    forward(200)
    left(90)
    forward(200)
    left(90)
    forward(200)
    end_fill()
    penup()

    setheading(0)
    forward(100)
    left(90)
    forward(100)
    pencolor('black')
    dot(170)
    left(180)
    forward(100)
    right(90)
    forward(100)
    setheading(270)


    # positioning of the body
    forward(-120)
    right(90)
    forward(-60)
    left(90)
    #### body
    width(5)
    fillcolor('Firebrick')
    begin_fill()
    pendown()
    right(45)
    circle(60, extent = 270)
    right(45)
    circle(50, extent = 70)
    left(30)
    circle(50, extent = 10 )
    left(30)
    circle(50, extent = 10 )
    left(30)
    circle(50, extent = 40 )
    right(190)
    circle(50, extent = 60 )
    left(30)
    circle(50, extent = 20 )
    left(30)
    circle(50, extent = 20 )
    left(70)
    forward(10)
    end_fill()
    penup()
    width(3)
    # positioning
    forward(70)

    #Brows
    fillcolor('black')
    begin_fill()
    pendown()
    right(140)
    forward(30)
    right(60)
    forward(30)
    left(80)
    forward(10)
    left(105)
    forward(35)
    left(50)
    forward(35)
    left(80)
    forward(10)
    end_fill()
    penup()

    #positining
    forward(3)
    right(90)
    forward(-56)
    left(90)

    #left eye
    fillcolor('white')
    begin_fill()
    pendown()
    left(100)
    circle(20, extent = 30)
    left(30)
    circle(20, extent = 40)
    left(30)
    circle(20, extent = 80)
    left(30)
    forward(2)
    end_fill()
    penup()


    #positioning
    right(90)
    forward(10)
    left(90)


    #right eye

    pendown()
    fillcolor('white')
    begin_fill()
    pendown()
    left(175)
    circle(20, extent = 30)
    left(30)
    circle(20, extent = 40)
    left(30)
    circle(20, extent = 80)
    left(30)
    forward(2)
    end_fill()
    penup()


    ## positioining
    setheading(180)
    forward(9)
    left(90)
    forward(15)

    #### Eye dot right
    pendown()
    dot(10)
    penup()

    ## positioining
    setheading(180)
    forward(35)
    left(90)
    forward(-4)

    #### Eye dot left
    pendown()
    dot(10)
    penup()

    #position

    setheading(270)
    forward(50)
    setheading(0)
    forward(57)
    setheading(90)

    # belly
    fillcolor('wheat')
    begin_fill()
    pendown()
    left(50)
    circle(50, extent = 94)
    setheading(0)
    right(38)
    circle(50, extent = 94)
    end_fill()
    penup()
    #### position
    left(90)
    forward(70)
    right(90)
    right(30)
    forward(18)
    left(30)
    ###beak
    width(3)
    fillcolor('Orange')
    begin_fill()
    pendown()
    left(170)
    circle(50, extent = 30)
    left(120)
    forward(30)
    left(120)
    forward(25)
    end_fill()
    penup()

    ##position
    setheading(270)
    forward(22)

    ## bottom part of beak
    width(2)
    fillcolor('Dark Orange')
    begin_fill()
    pendown()
    left(45)
    forward(10)
    left(100)
    forward(12)
    left(135)
    forward(12)
    end_fill()
    penup()

    ### Crack
    fillcolor('lightblue')
    forward(110)
    left(90)
    forward(50)
    setheading(45)
    pendown()
    begin_fill()
    width(5)
    pencolor('black')
    forward(40)
    left(45)
    forward(20)
    right(30)
    forward(40)
    left(40)
    forward(30)
    right(60)
    forward(40)
    right(30)
    forward(40)
    right(20)
    forward(20)
    left(60)
    forward(75)
    left(180)
    forward(75)
    left(30)
    forward(20)
    left(45)
    forward(20)
    right(42)
    forward(30)
    right(25)
    forward(50)
    right(30)
    forward(50)
    right(45)
    forward(45)
    end_fill()
    penup()

def broken_one_by_one(): ### Broken Chuck

    ### Tile outline
    setheading(0)
    pendown()
    setheading(0)
    width(3)
    pencolor('black')
    fillcolor('beige')
    begin_fill()
    forward(100)
    left(90)
    forward(100)
    left(90)
    forward(100)
    left(90)
    forward(100)
    end_fill()
    penup()

    ### Positioning
    setheading(0)
    forward(20)
    left(90)
    forward(10)

    ### Body
    width(3)
    fillcolor('yellow')
    begin_fill()
    pendown()
    right(100)
    circle(200, extent = 20)
    left(90)
    circle(200, extent = 20)
    left(100)
    circle(200, extent = 24)
    left(120)
    forward(5)
    end_fill()

    ### Positioning
    penup()
    setheading(0)
    forward(50)
    left(90)
    forward(60)

    ### Spikey Hair
    fillcolor('black')
    begin_fill()
    pendown()
    right(50)
    circle(20, extent = 70)
    right(220)
    forward(10)
    right(110)
    circle(30, extent = 40)
    left(120)
    forward(15)
    right(150)
    circle(30, extent = 40)
    left(130)
    forward(20)
    end_fill()

    ### Positioning
    penup()
    setheading(0)
    right(90)
    forward(25)
    right(90)
    forward(5)

    ### Eyes
    penup()
    right(90)
    forward(-8)
    pendown()
    pencolor('black')
    dot(15)
    pencolor('white')
    dot(10)
    pencolor('black')
    dot(5)
    penup()
    right(90)
    forward(20)
    pendown()
    pencolor('black')
    dot(15)
    pencolor('white')
    dot(10)
    pencolor('black')
    dot(5)
    penup()

    ### Positioning
    left(180)
    forward(20)
    left(90)
    forward(12)
    left(100)

    ### Beak
    fillcolor('orange')
    begin_fill()
    pendown()
    width(2)
    forward(15)
    right(20)
    forward(15)
    setheading(180)
    left(10)
    forward(10)
    left(90)
    forward(10)
    left(10)
    forward(10)
    setheading(180)
    right(40)
    forward(32)
    end_fill()

    ### Positioning
    penup()
    setheading(180)
    forward(9)
    setheading(90)
    forward(25)

    ### Left eye brow
    pendown()
    pencolor('firebrick')
    fillcolor('firebrick')
    begin_fill()
    setheading(0)
    forward(20)
    left(90)
    forward(2)
    left(90)
    forward(20)
    left(90)
    forward(2)
    end_fill()

    ### Positioning
    penup()
    setheading(0)
    forward(20)
    setheading(90)
    forward(5)
    setheading(0)


    ### Right eye brow
    pendown()
    pencolor('firebrick')
    fillcolor('firebrick')
    begin_fill()
    setheading(0)
    forward(20)
    left(90)
    forward(2)
    left(90)
    forward(20)
    left(90)
    forward(2)
    end_fill()
    penup()

    ### Cracked poriton
    right(90)
    forward(60)
    left(90)
    forward(50)
    setheading(0)
    width(5)
    pencolor('black')
    left(45)
    pendown()
    fillcolor('light blue')
    begin_fill()
    forward(20)
    left(45)
    forward(20)
    right(25)
    forward(25)
    left(25)
    forward(10)
    right(65)
    forward(40)
    setheading(90)
    forward(10)
    setheading(270)
    forward(10)
    left(55)
    forward(40)
    setheading(270)
    forward(40)
    setheading(180)
    forward(60)
    left(25)
    forward(20)
    end_fill()
    penup()

def broken_horizontal(): ### Broken Bad Piggie

    ### Tile outline
    setheading(0)
    fillcolor('sandy brown')
    begin_fill()
    width(3)
    pendown()
    forward(200)
    left(90)
    forward(100)
    left(90)
    forward(200)
    left(90)
    forward(100)
    end_fill()
    penup()

    ### Positioning
    setheading(0)
    forward(100)
    setheading(90)
    forward(5)

    ### Body
    pencolor('dark green')
    fillcolor('lawn green')
    begin_fill()
    pendown()
    setheading(0)
    circle(40)
    end_fill()

    ### Ears
    penup()
    circle(40, extent = 130)
    pendown()
    fillcolor('lawn green')
    begin_fill()
    circle(-10)
    end_fill()
    penup()
    circle(40, extent = 90)
    pendown()
    fillcolor('lawn green')
    begin_fill()
    circle(-10)
    end_fill()
    penup()
    circle(40, extent = 150)
    setheading(90)
    forward(40)

    ### Eyes
    setheading(180)
    forward(30)
    pendown()
    pencolor('green')
    dot(20)
    pencolor('white')
    dot(15)
    pencolor('black')
    dot(5)
    penup()
    setheading(0)
    forward(25)
    pendown()
    pencolor('green')
    dot(30)
    pencolor('yellow green')
    dot(25)

    ### Nose holes
    penup()
    setheading(180)
    forward(5)
    pendown()
    pencolor('dark green')
    dot(10)
    setheading(0)
    penup()
    forward(10)
    pendown()
    dot(10)
    penup()
    forward(20)
    pendown()
    dot(20)
    pencolor('white')
    dot(15)
    pencolor('black')
    dot(5)

    ### Positioning
    pencolor('dark green')
    penup()
    setheading(180)
    forward(25)
    setheading(270)
    forward(20)
    dot(15)
    penup()
    forward(-5)
    pencolor('white')
    pendown()
    dot(5)
    setheading(0)
    forward(-5)
    dot(5)
    forward(10)
    dot(5)
    penup()

    ### TNT BOX
    setheading(180)
    forward(100)
    setheading(270)
    pencolor('saddle brown')
    fillcolor('burlywood')
    begin_fill()
    pendown()
    forward(25)
    left(90)
    forward(25)
    left(90)
    forward(25)
    left(90)
    forward(25)
    penup()
    left(90)
    forward(15)
    left(90)
    forward(5)
    end_fill()
    pencolor('red')
    write('TNT')

    ### TNT BOX
    penup()
    forward(15)
    left(90)
    forward(60)
    setheading(270)
    pencolor('saddle brown')
    fillcolor('burlywood')
    begin_fill()
    pendown()
    forward(25)
    left(90)
    forward(25)
    left(90)
    forward(25)
    left(90)
    forward(25)
    penup()
    left(90)
    forward(15)
    left(90)
    forward(5)
    end_fill()
    pencolor('red')
    write('TNT')

    ### TNT BOX
    penup()
    forward(120)
    right(90)
    forward(30)
    pencolor('saddle brown')
    fillcolor('burlywood')
    begin_fill()
    pendown()
    forward(25)
    left(90)
    forward(25)
    left(90)
    forward(25)
    left(90)
    forward(25)
    penup()
    left(90)
    forward(15)
    left(90)
    forward(5)
    end_fill()
    pencolor('red')
    write('TNT')

    ### Cracked portion
    setheading(180)
    forward(156)
    setheading(0)
    fillcolor('light blue')
    begin_fill()
    pendown()
    width(5)
    pencolor('black')
    forward(20)
    left(65)
    forward(40)
    right(25)
    forward(40)
    right(35)
    forward(40)
    right(65)
    forward(30)
    left(65)
    forward(30)
    right(25)
    forward(20)
    left(25)
    forward(24)
    left(180)
    forward(24)
    left(65)
    forward(40)
    right(60)
    forward(20)
    right(20)
    forward(60)
    left(30)
    forward(30)
    right(35)
    forward(30)
    penup()
    end_fill()

def broken_vertical(): ### Broken The Blues

    # Tile outline
    setheading(0)
    fillcolor('light coral')
    begin_fill()
    width(3)
    pendown()
    forward(100)
    left(90)
    forward(200)
    left(90)
    forward(100)
    left(90)
    forward(200)
    end_fill()
    penup()

    ### Positioning
    setheading(0)
    forward(30)
    left(90)
    forward(30)

    ### Body
    fillcolor('steel blue')
    begin_fill()
    pendown()
    setheading(0)
    right(19)
    circle(50, extent = 110)
    setheading(90)
    forward(40)
    circle(50, extent = 50)
    right(40)
    circle(20, extent = 90)
    left(30)
    circle(20, extent = 20)
    left(30)
    circle(20, extent = 20)
    left(40)
    forward(15)
    right(180)
    forward(15)
    circle(50, extent = 20)
    left(30)
    circle(20, extent = 30)
    left(30)
    circle(20, extent = 30)
    left(50)
    forward(10)
    right(121)
    circle(80, extent = 94)
    end_fill()

    ### Positioning
    penup()
    setheading(90)
    forward(90)
    setheading(0)
    forward(10)

    ### Left eye
    pendown()
    dot(30)
    pencolor('white')
    dot(25)
    penup()
    forward(7)
    pendown()
    pencolor('black')
    dot(10)

    ### Positioning
    penup()
    forward(30)

    ### Right eye
    pendown()
    dot(30)
    pencolor('white')
    dot(25)
    penup()
    forward(7)
    pendown()
    pencolor('black')
    dot(10)

    ### Positioning
    penup()
    setheading(270)
    forward(15)
    setheading(180)
    forward(45)

    ### Orange bit under beak
    fillcolor('firebrick')
    begin_fill()
    pendown()
    left(130)
    circle(50, extent = 60)
    left(90)
    forward(18)
    left(80)
    forward(40)
    end_fill()

    ### Positioning
    penup()
    left(55)
    forward(20)
    left(170)

    ### Beak
    fillcolor('orange')
    begin_fill()
    pendown()
    forward(30)
    right(75)
    forward(45)
    setheading(180)
    forward(60)
    end_fill()
    fillcolor('orange')
    begin_fill()
    setheading(270)
    left(30)
    forward(20)
    left(80)
    forward(42)
    end_fill()
    penup()

    # Cracked portion
    forward(20)
    setheading(180)
    width(5)
    pendown()
    fillcolor('light blue')
    begin_fill()
    left(65)
    forward(80)
    right(25)
    forward(20)
    right(55)
    forward(30)
    right(90)
    forward(50)
    left(30)
    forward(20)
    right(30)
    forward(50)
    left(30)
    forward(40)
    right(90)
    forward(30)
    right(90)
    forward(50)
    left(90)
    forward(28)
    penup()
    end_fill()
   

# Fill the grid with tiles as per the provided dataset  


def tessellate(random_pattern):
    #######################################################################
    ### This first section creates the while loop to acess the entire list generated by random_pattern
    n = len(random_pattern) 
    while n >= 0:
        position = 0
        while position >= 0:
            length = len(random_pattern[position])
            position = position + 1

            
            #######################################################################

            
            ### This section is for the random patterns featuring lists of legnth 3
            if length == 3: 
                if (((random_pattern[position - 1])[0])[1]) == (((random_pattern[position - 1])[1])[1]): 
                    if str(((random_pattern[position - 1])[2])) == 'O': # Case where pattern ends with O.
                        ### Co-ordinates x_pos (horizontal position) and y_pos (vertical position) determination. ###
                        for x_pos in random_pattern:
                            x_pos = ((ord(((random_pattern[position - 1])[0])[0])) - 60) 
                        for y_pos in random_pattern:
                            y_pos = int((((random_pattern[position - 1])[0])[1])) 
                        ### Using x_pos and y_pos a linear regression is formed for each caclulating the positions on the grid. ###
                        goto(((100*x_pos) - 1000), ((int(y_pos) *100) - 450)) 
                        pencolor('black')                                     
                        horizontal() # What it draws
                    elif str(((random_pattern[position - 1])[2])) == 'X': # Case where pattern ends with X.
                        ### Co-ordinates x_pos (horizontal position) and y_pos (vertical position) determination. ###
                        for x_pos in random_pattern:
                            x_pos = ((ord(((random_pattern[position - 1])[0])[0])) - 60) 
                        for y_pos in random_pattern:
                            y_pos = int((((random_pattern[position - 1])[0])[1])) 
                        ### Using x_pos and y_pos a linear regression is formed for each caclulating the positions on the grid. ###
                        goto(((100*x_pos) - 1000), ((int(y_pos) *100) - 450)) 
                        pencolor('black')                                     
                        broken_horizontal() # What it draws
                else: # Since lists in random pattern with a length of 3 feature two cases, a horizontal and vertical pattern else statement is needed for second case (vertical).
                    if str(((random_pattern[position - 1])[2])) == 'O': # Case where pattern ends with O.
                        ### Co-ordinates x_pos (horizontal position) and y_pos (vertical position) determination. ###
                        for x_pos in random_pattern:
                            x_pos = ((ord(((random_pattern[position - 1])[0])[0])) - 60) 
                        for y_pos in random_pattern:
                            y_pos = int((((random_pattern[position - 1])[0])[1]))
                        ### Using x_pos and y_pos a linear regression is formed for each caclulating the positions on the grid. ###
                        goto(((100*x_pos) - 1000), ((int(y_pos) *100) - 450))
                        pencolor('black')
                        vertical() # What it draws
                    elif str(((random_pattern[position - 1])[2])) == 'X': # Case where pattern ends with X.
                        ### Co-ordinates x_pos (horizontal position) and y_pos (vertical position) determination. ###
                        for x_pos in random_pattern:
                            x_pos = ((ord(((random_pattern[position - 1])[0])[0])) - 60) 
                        for y_pos in random_pattern:
                            y_pos = int((((random_pattern[position - 1])[0])[1]))
                        ### Using x_pos and y_pos a linear regression is formed for each caclulating the positions on the grid. ###
                        goto(((100*x_pos) - 1000), ((int(y_pos) *100) - 450))
                        pencolor('black')
                        broken_vertical() # What it draws

                        
            #######################################################################

            ### This section is for the random patterns featuring lists of legnth 5            
            elif length == 5:
                if str(((random_pattern[position - 1])[4])) == 'O': # Case where pattern ends with O.
                    ### Co-ordinates x_pos (horizontal position) and y_pos (vertical position) determination. ###
                    for x_pos in random_pattern:
                        x_pos = ((ord(((random_pattern[position - 1])[0])[0])) - 60) 
                    for y_pos in random_pattern:
                        y_pos = int((((random_pattern[position - 1])[0])[1]))
                    ### Using x_pos and y_pos a linear regression is formed for each caclulating the positions on the grid. ###
                    goto(((100*x_pos) - 1000), ((int(y_pos) *100) - 450))
                    pencolor('black')
                    two_by_two() # What it draws
                elif str(((random_pattern[position - 1])[4])) == 'X': # Case where pattern ends with X.
                    ### Co-ordinates x_pos (horizontal position) and y_pos (vertical position) determination. ###
                    for x_pos in random_pattern:
                        x_pos = ((ord(((random_pattern[position - 1])[0])[0])) - 60) 
                    for y_pos in random_pattern:
                        y_pos = int((((random_pattern[position - 1])[0])[1]))
                    ### Using x_pos and y_pos a linear regression is formed for each caclulating the positions on the grid. ###
                    goto(((100*x_pos) - 1000), ((int(y_pos) *100) - 450))
                    pencolor('black')
                    broken_tile_two_by_two() # What it draws

                    
             ######################################################################

            ### This section is for the random patterns featuring lists of legnth 2        
            elif length == 2:
                if str(((random_pattern[position - 1])[1])) == 'O': # Case where pattern ends with O.
                    ### Co-ordinates x_pos and y_pos determination.
                    for x_pos in random_pattern:
                        x_pos = ((ord(((random_pattern[position - 1])[0])[0])) - 60) # Finds x position using ord of the first letter of the list.
                    for y_pos in random_pattern:
                        y_pos = int((((random_pattern[position - 1])[0])[1]))
                    ### Using x_pos and y_pos a linear regression is formed for each caclulating the positions on the grid. ###
                    goto(((100*x_pos) - 1000), ((int(y_pos) *100) - 450))
                    pencolor('black')
                    one_by_one() # What it draws
                elif str(((random_pattern[position - 1])[1])) == 'X': # Case where pattern ends with X.
                    ### Co-ordinates x_pos and y_pos determination.
                    for x_pos in random_pattern:
                        x_pos = ((ord(((random_pattern[position - 1])[0])[0])) - 60) # Finds x position using ord of the first letter of the list.
                    for y_pos in random_pattern:
                        y_pos = int((((random_pattern[position - 1])[0])[1]))
                    ### Using x_pos and y_pos a linear regression is formed for each caclulating the positions on the grid. ###
                    goto(((100*x_pos) - 1000), ((int(y_pos) *100) - 450))
                    pencolor('black')
                    broken_one_by_one() # What it draws
        n = n - 1

# ord() converts the letters
# chr() converts the numbers





#
#--------------------------------------------------------------------#



#-----Main Program---------------------------------------------------#
#
# This main program sets up the background, ready for you to start
# drawing your solution.  Do not change any of this code except
# as indicated by the comments marked '*****'.
#

# Set up the drawing canvas
# ***** You can change the background and line colours, and choose
# ***** whether or not to draw the grid and mark the places for the
# ***** legend, by providing arguments to this function call
create_drawing_canvas()
tracer(False)
penup()

goto(-750,100)
two_by_two()
penup()
goto(-750,70)
pendown()
pencolor('black')
write('Red', font =("Arial", 18, "normal"))
penup()

goto(-700,-100)
one_by_one()
goto(-700,-130)
pendown()
pencolor('black')
write('Chuck', font =("Arial", 18, "normal"))
penup()

goto(600, 100)
pencolor('black')
vertical()
penup()
goto(600,70)
pendown()
pencolor('black')
write('The Blues', font =("Arial", 18, "normal"))
penup()

goto(550, -100)
horizontal()
penup()
goto(550,-130)
pendown()
pencolor('black')
write('Bad Piggies', font =("Arial", 18, "normal"))
penup()


# Control the drawing speed
# ***** Change the following argument if you want to adjust
# ***** the drawing speed
speed('fastest')

# Decide whether or not to show the drawing being done step-by-step
# ***** Set the following argument to False if you don't want to wait
# ***** forever while the cursor moves slowly around the screen
tracer(False)

# Give the drawing canvas a title
# ***** Replace this title with a description of your solution's theme
# ***** and its tiles
title("Angry Birds & Piggies")

### Call the student's function to follow the path
### ***** While developing your program you can call the tessellate
### ***** function with one of the "fixed" data sets, but your
### ***** final solution must work with "random_pattern()" as the
### ***** argument.  Your tessellate function must work for any data
### ***** set that can be returned by the random_pattern function.
#tessellate(fixed_pattern_0) # <-- used for code development only, not marking
tessellate(random_pattern()) # <-- used for assessment

# Exit gracefully
# ***** Change the default argument to False if you want the
# ***** cursor (turtle) to remain visible at the end of the
# ***** program as a debugging aid
release_drawing_canvas()

#
#--------------------------------------------------------------------#





