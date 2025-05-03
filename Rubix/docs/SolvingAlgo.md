
Important note, There is no such thing as rotating the faces
The cube only has one orientation at all times and is defined in Cube.h
# NEEDS TO BE VERIFIED USING A REAL CUBE

# Brute Force DFS
- this is the current implementation
# ---------SOLVING METHOD ------------
- make the cross, center White is the top(#0) with side faces(#1-4)
    Compare the appropriate face indices with center
    straight IDFS with to find face[0] matching conditions
    **implmenting the search for this was tricky and caused lots of room for optimization of speed**

- Create target states for each "step" of solving and IDFS until you find that state and move on.


# Trying out THistlewaith's solving using a simple condition state parameter.
- using the heuristics of Thistle break into 4 groups and the conditions needed
- brute force IDFS search with a condition for paratmers
- progress through the conditions until end.
VOILA!