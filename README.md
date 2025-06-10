![Video](showcase/sneak_peak.gif)
*A Falling Sand engine with DirectX*  
*This is an old project(2022) and the quality of code does not represent how I develop today*



# Table of contents

1. [Set-up](#set-up)
2. [ChilliFramework](#chilliframework)
2. [Description](#usage)
3. [Optimisations](#development)

## Set-up

Make sure you have `Visual Studio Community Edition`. This 
project is quite old(2022) and developed on the `2017` edition. 


After that, just search for the VS Community file and open it.
## ChilliFramework
The project was built on top of the `ChilliFramework`. A DirectX framework that `is lower level than OpenGL` and provides direct access to the window pixel buffer. I had to create my own drawing functions which was a way of realising how math is actually very cool.
## Description

The idea of a `Falling Sand` game is not that complicated. You transform a selected area into a matrix of elements of a fixed size, and each element has a set of `rules` as to how to interact with `other` matrix elements around it.

For a quick overview as to how these rules work, you can watch this [video](https://www.youtube.com/watch?v=VLZjd_Y1gJ8).

The engine supports a basic variety of elements: 
- Water
- Stone
- Fire
- Wood
- Acid
- Steam
- Smoke
- Toxic Gas
- Snow

## Optimisations

The real challange does not come from implementing the interaction between elements, but from scaling and optimising.  
  
Just on the GIF shown above, there are `129600 particles` that need to be rendered, and `9X` the amount that need to be updated off screen.

The use of:
- separation of concerns
- const correctness
- copy vs reference vs move
- minimalizing the amount of checks I have
- switching from a class composition approach to inheritence and back to composition
- creating a chunk system for efficient particle updates
- avoding `std::function` overhead

Has made a huge impact on performance. 

While the system is not perfect, and can benefit from a variety of optimisations (Assembly/new data arrangements/class relations), it was a challanging and fun project. Hope I will go back to it one day.

