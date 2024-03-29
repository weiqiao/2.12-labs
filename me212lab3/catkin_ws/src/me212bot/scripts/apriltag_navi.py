#!/usr/bin/python

# 2.12 Lab 3 AprilTag Navigation: use AprilTag to get current robot (X,Y,Theta) in world frame, and to navigate to target (X,Y,Theta)
# Peter Yu Sept 2016

import rospy
import tf
import numpy as np
import threading
import serial
import tf.transformations as tfm

from me212bot.msg import WheelCmdVel
from apriltags.msg import AprilTagDetections
from helper import transformPose, pubFrame, cross2d, lookupTransform, pose2poselist, invPoselist, diffrad, poselist2pose


rospy.init_node('apriltag_navi', anonymous=True)
lr = tf.TransformListener()
br = tf.TransformBroadcaster()
    
def main():
    apriltag_sub = rospy.Subscriber("/apriltags/detections", AprilTagDetections, apriltag_callback, queue_size = 1)
    

    constant_vel_loop()

#    rospy.sleep(1)
    
    constant_vel = False
    if constant_vel:
        thread = threading.Thread(target = constant_vel_loop)
    else:
        thread = threading.Thread(target = navi_loop)
    thread.start()
    
    rospy.spin()

## sending constant velocity (Need to modify for Task 1)
def constant_vel_loop():

    pub = rospy.Publisher('/cmdvel',WheelCmdVel,queue_size=1)
    rate = rospy.Rate(100)

    wcv = WheelCmdVel()
    while not rospy.is_shutdown():
	wcv.desiredWV_R = 0.1
	wcv.desiredWV_L = 0.2
	pub.publish(wcv)
	rate.sleep()
'''
    ## velcmd_pub = ??
    rate = rospy.Rate(100) # 100hz
    
    while not rospy.is_shutdown() :
        ## wcv = ??
        ## wcv.desiredWV_R = ??
        ## wcv.desiredWV_L = ??
        
        ## velcmd_pub.publish(??) 
        
        rate.sleep() 
'''

## apriltag msg handling function (Need to modify for Task 2)
def apriltag_callback(data):
    # use apriltag pose detection to find where is the robot
    for detection in data.detections:
        if detection.id == 0:   # tag id is the correct one
            poselist_tag_cam = pose2poselist(detection.pose)


	   # poselist_cam_base_bylookup = lookupTransform(lr, sourceFrame = '/camera', targetFrame = '/robot_base')

            poselist_tag_base = transformPose(lr, poselist_tag_cam, sourceFrame = '/camera', targetFrame = '/robot_base')
	    #pose_tag_base = poselist2pose(poselist_tag_base)
            poselist_base_tag = invPoselist(poselist_tag_base)
            poselist_base_map = transformPose(lr, poselist_base_tag,sourceFrame = '/apriltag', targetFrame = '/map')
            
	    pubFrame(br, pose = poselist_base_map, frame_id = '/robot_base', parent_frame_id = '/map')


## navigation control loop (No need to modify)
def navi_loop():
    velcmd_pub = rospy.Publisher("/cmdvel", WheelCmdVel, queue_size = 1)
    target_pose2d = [0.25, 0, np.pi]
    rate = rospy.Rate(100) # 100hz
    
    wcv = WheelCmdVel()
    
    arrived = False
    arrived_position = False
    
    while not rospy.is_shutdown() :
        # 1. get robot pose
        robot_pose3d = lookupTransform(lr, '/map', '/robot_base')
        
        if robot_pose3d is None:
            print '1. Tag not in view, Stop'
            wcv.desiredWV_R = 0  # right, left
            wcv.desiredWV_L = 0
            velcmd_pub.publish(wcv)  
            rate.sleep()
            continue
        
        robot_position2d  = robot_pose3d[0:2]
        target_position2d = target_pose2d[0:2]
        
        robot_yaw    = tfm.euler_from_quaternion(robot_pose3d[3:7]) [2]
        robot_pose2d = robot_position2d + [robot_yaw]
        
        # 2. navigation policy
        # 2.1 if       in the target, stop
        # 2.2 else if  close to target position, turn to the target orientation
        # 2.3 else if  in the correct heading, go straight to the target position,
        # 2.4 else     turn in the direction of the target position
        
        pos_delta         = np.array(target_position2d) - np.array(robot_position2d)
        robot_heading_vec = np.array([np.cos(robot_yaw), np.sin(robot_yaw)])
        heading_err_cross = cross2d( robot_heading_vec, pos_delta / np.linalg.norm(pos_delta) )
        
        # print 'robot_position2d', robot_position2d, 'target_position2d', target_position2d
        # print 'pos_delta', pos_delta
        # print 'robot_yaw', robot_yaw
        # print 'norm delta', np.linalg.norm( pos_delta ), 'diffrad', diffrad(robot_yaw, target_pose2d[2])
        # print 'heading_err_cross', heading_err_cross
        
        if arrived or (np.linalg.norm( pos_delta ) < 0.08 and np.fabs(diffrad(robot_yaw, target_pose2d[2]))<0.05) :
            print 'Case 2.1  Stop'
            wcv.desiredWV_R = 0  
            wcv.desiredWV_L = 0
            arrived = True
        elif np.linalg.norm( pos_delta ) < 0.08:
            arrived_position = False
            if diffrad(robot_yaw, target_pose2d[2]) > 0:
                print 'Case 2.2.1  Turn right slowly'      
                wcv.desiredWV_R = -0.05 
                wcv.desiredWV_L = 0.05
            else:
                print 'Case 2.2.2  Turn left slowly'
                wcv.desiredWV_R = 0.05  
                wcv.desiredWV_L = -0.05
                
        elif arrived_position or np.fabs( heading_err_cross ) < 0.2:
            print 'Case 2.3  Straight forward'  
            wcv.desiredWV_R = 0.1
            wcv.desiredWV_L = 0.1
        else:
            if heading_err_cross < 0:
                print 'Case 2.4.1  Turn right'
                wcv.desiredWV_R = -0.1
                wcv.desiredWV_L = 0.1
            else:
                print 'Case 2.4.2  Turn left'
                wcv.desiredWV_R = 0.1
                wcv.desiredWV_L = -0.1
                
        velcmd_pub.publish(wcv)  
        
        rate.sleep()

if __name__=='__main__':
    main()
    
