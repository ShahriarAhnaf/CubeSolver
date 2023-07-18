
Important note, There is no such thing as rotating the faces
The cube only has one orientation at all times and is defined in Cube.h
# NEEDS TO BE VERIFIED USING A REAL CUBE

# Brute Force DFS
- this is the current implementation
# ---------SOLVING METHOD ------------
- make the cross, center White is the top(#0) with side faces(#1-4)
    Compare the appropriate face indices with center
    straight IDFS with to find face[0] matching conditions
    **implmenting the search for this will be the scariest prolly**

- finish the white face corner while matching all 3 faces with their center colors
    This will finish the first layer
- Conditional algorithm blasting based on preloaded bottom colour matching scheme
    Gonna be a huge case statement prolly
    This finish the second layer 
- More conditional schemes but with the bottom, looping algorithms until it sticks
VOILA!