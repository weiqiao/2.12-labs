#!/usr/bin/python

from planner import fk1, fk
import numpy as np

def ccw(A,B,C):
    return (C[1]-A[1]) * (B[0]-A[0]) > (B[1]-A[1]) * (C[0]-A[0])

# Return true if line segments AB and CD intersect
def intersect(A,B,C,D):
    return ccw(A,C,D) != ccw(B,C,D) and ccw(A,B,C) != ccw(A,B,D)

def in_collision(q, obstacle_segs):
    arm_segments = [( (0,0), fk1(q) ), ( fk1(q), fk(q))]
    
    incollision = False
    for seg in obstacle_segs:
        for arm in arm_segments:
            if intersect(arm[0],arm[1],seg[0],seg[1]):
                incollision = True
                break
            
    return incollision
    ## return True if there are segments from arm_segments and segments from obstacle_segs intersect

if __name__=="__main__":
    
    obstacle_segs = [ [[0.0,0.0], [100,100]], [[0.0,0.0], [200,100]]]  # line segs ((x1,z1)--(x2,z2))
    
    '''
    A = [0.2,0.1,-0.1,-0.2,0,0.2,0.1,-0.1,-0.2,0]
    B = [0.1,-0.1,-0.2,0,-0.2,0,0.2,0.1,-0.1,0.04]
    C = [-0.2,0,-0.2,0,0.2,0.1,0.2,0.1,-0.1,0.04]
    D = [-0.2,0,0.2,0.1,-0.1,-0.2,0,0.2,0.1,0.2,0.1]
    for a in range(10):
        obstacle_segs = obstacle_segs + [ [[0.0+A[a],0.2*a+B[a]], [0.2*a+C[a],0.4*a+D[a]]] ]
        #print obstacle_segs
    '''
    print in_collision( [0,0], obstacle_segs)                       # False
    print in_collision( [np.pi /4, -np.pi *3/4], obstacle_segs)     # True
    print in_collision( [-0.45709828817786735, -1.4971869034039356], obstacle_segs) # False
