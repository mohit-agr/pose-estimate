#include <ros/ros.h>
#include <tf/transform_listener.h>
#include <tf/transform_datatypes.h>
#include <tf/transform_broadcaster.h>


int main(int argc, char** argv) {	
	ros::init(argc, argv, "pose_estimator");
	ros::NodeHandle nh;
	ros::NodeHandle nhp("~");

	std::string parent_, child_, origin_;

	parent_ = "/master";
	child_ = "/slave";
	origin_ = "/world";

	nhp.param("parent_frame", parent_, parent_);	
	nhp.param("child_frame", child_, child_);
	nhp.param("origin_frame", origin_, origin_);

	ros::Publisher sTF_pub = nh.advertise<geometry_msgs::TransformStamped>("/stampedTF", 10);
	ros::Publisher pose_pub = nh.advertise<geometry_msgs::Transform>("/pose", 10);
	tf::TransformListener Lnr_W_mtr, Lnr_W_slv;

	ros::Rate rate(10.0);
	geometry_msgs::TransformStamped pose_msg;
	tf::TransformBroadcaster br;

	while(nh.ok()) {
		tf::StampedTransform T_W_mtr, T_W_slv, T_mtr_slv;
		try {
			Lnr_W_mtr.lookupTransform(origin_, parent_, ros::Time(0), T_W_mtr);
			Lnr_W_slv.lookupTransform(origin_, child_, ros::Time(0), T_W_slv);
		} catch (tf::TransformException &ex) {
		    ROS_ERROR("%s",ex.what());
		    ros::Duration(1.0).sleep();
		    continue;
	    }

	    T_mtr_slv = tf::StampedTransform( (T_W_mtr.inverse() * T_W_slv), 
    									T_W_mtr.stamp_,
    									parent_ + "/body",
    									child_ + "_est");

	    tf::transformStampedTFToMsg(T_mtr_slv, pose_msg);
	    sTF_pub.publish(pose_msg);
	    br.sendTransform(T_mtr_slv);
	}
	return 0;
}
