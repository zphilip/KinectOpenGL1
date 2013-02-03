/*
 * Copyright (c) 2008, Willow Garage, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Willow Garage, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/** \author Tully Foote */

/* Initial WIN32 port by Hozefa Indorewala, Robotics Equipment Corporation GmbH, www.servicerobotics.eu */

#ifdef WIN32
#define NOMINMAX
#else
#include <sys/time.h>
#endif //WIN32

#include "tf/tf.h"
#include "ros/assert.h"
#include "ros/ros.h"
#include "angles/angles.h"

using namespace tf;

// Must provide storage for non-integral static const class members.
// Otherwise you get undefined symbol errors on OS X (why not on Linux?).
// Thanks to Rob for pointing out the right way to do this.
//#ifdef WIN32
const double Transformer::DEFAULT_CACHE_TIME = 10.0;
//#else
//const double tf::Transformer::DEFAULT_CACHE_TIME;
//#endif //WIN32

std::string assert_resolved(const std::string& prefix, const std::string& frame_id)
{
  if (frame_id.size() > 0)
    if (frame_id[0] != '/')
      ROS_DEBUG("TF operating on not fully resolved frame id %s, resolving using local prefix %s", frame_id.c_str(), prefix.c_str());
  return tf::resolve(prefix, frame_id);
};

std::string tf::resolve(const std::string& prefix, const std::string& frame_name)
{
  //  printf ("resolveping prefix:%s with frame_name:%s\n", prefix.c_str(), frame_name.c_str());
  if (frame_name.size() > 0)
    if (frame_name[0] == '/')
    {
      return frame_name;
    }
  if (prefix.size() > 0)
  {
    if (prefix[0] == '/')
    {
      std::string composite = prefix;
      composite.append("/");
      composite.append(frame_name);
      return composite;
    }
    else
    {
      std::string composite;
      composite = "/";
      composite.append(prefix);
      composite.append("/");
      composite.append(frame_name);
      return composite;
    }

  }
  else
 {
    std::string composite;
    composite = "/";
    composite.append(frame_name);
    return composite;
  }
};



Transformer::Transformer(bool interpolating,
                                ros::Duration cache_time):
  cache_time(cache_time),
  interpolating (interpolating), 
  using_dedicated_thread_(false),
  fall_back_to_wall_time_(false)
{
  max_extrapolation_distance_.fromNSec(DEFAULT_MAX_EXTRAPOLATION_DISTANCE);
  frameIDs_["NO_PARENT"] = 0;
  frames_.push_back(NULL);// new TimeCache(interpolating, cache_time, max_extrapolation_distance));//unused but needed for iteration over all elements
  frameIDs_reverse.push_back("NO_PARENT");

  return;
}

Transformer::~Transformer()
{
  /* deallocate all frames */
  boost::mutex::scoped_lock(frame_mutex_);
  for (std::vector<TimeCache*>::iterator  cache_it = frames_.begin(); cache_it != frames_.end(); ++cache_it)
  {
    delete (*cache_it);
  }

};


void Transformer::clear()
{
  boost::mutex::scoped_lock(frame_mutex_);
  if ( frames_.size() > 1 )
  {
    for (std::vector< TimeCache*>::iterator  cache_it = frames_.begin() + 1; cache_it != frames_.end(); ++cache_it)
    {
      (*cache_it)->clearList();
    }
  }
}


bool Transformer::setTransform(const StampedTransform& transform, const std::string& authority)
{

  StampedTransform mapped_transform((btTransform)transform, transform.stamp_, transform.frame_id_, transform.child_frame_id_);
  mapped_transform.child_frame_id_ = assert_resolved(tf_prefix_, transform.child_frame_id_);
  mapped_transform.frame_id_ = assert_resolved(tf_prefix_, transform.frame_id_);

 
  bool error_exists = false;
  if (mapped_transform.child_frame_id_ == mapped_transform.frame_id_)
  {
    ROS_ERROR("TF_SELF_TRANSFORM: Ignoring transform from authority \"%s\" with frame_id and child_frame_id  \"%s\" because they are the same",  authority.c_str(), mapped_transform.child_frame_id_.c_str());
    error_exists = true;
  }

  if (mapped_transform.child_frame_id_ == "/")//empty frame id will be mapped to "/"
  {
    ROS_ERROR("TF_NO_CHILD_FRAME_ID: Ignoring transform from authority \"%s\" because child_frame_id not set ", authority.c_str());
    error_exists = true;
  }

  if (mapped_transform.frame_id_ == "/")//empty parent id will be mapped to "/"
  {
    ROS_ERROR("TF_NO_FRAME_ID: Ignoring transform with child_frame_id \"%s\"  from authority \"%s\" because frame_id not set", mapped_transform.child_frame_id_.c_str(), authority.c_str());
    error_exists = true;
  }
#ifdef WIN32
  if (_isnan(mapped_transform.getOrigin().x()) || _isnan(mapped_transform.getOrigin().y()) || _isnan(mapped_transform.getOrigin().z())||
      _isnan(mapped_transform.getRotation().x()) || _isnan(mapped_transform.getRotation().y()) || _isnan(mapped_transform.getRotation().z()) || _isnan(mapped_transform.getRotation().w()))
#else
  if (std::isnan(mapped_transform.getOrigin().x()) || std::isnan(mapped_transform.getOrigin().y()) || std::isnan(mapped_transform.getOrigin().z())||
      std::isnan(mapped_transform.getRotation().x()) ||       std::isnan(mapped_transform.getRotation().y()) ||       std::isnan(mapped_transform.getRotation().z()) ||       std::isnan(mapped_transform.getRotation().w()))
#endif //WIN32
  {
    ROS_ERROR("TF_NAN_INPUT: Ignoring transform for child_frame_id \"%s\" from authority \"%s\" because of a nan value in the transform (%f %f %f) (%f %f %f %f)",
              mapped_transform.child_frame_id_.c_str(), authority.c_str(),
              mapped_transform.getOrigin().x(), mapped_transform.getOrigin().y(), mapped_transform.getOrigin().z(),
              mapped_transform.getRotation().x(), mapped_transform.getRotation().y(), mapped_transform.getRotation().z(), mapped_transform.getRotation().w()
              );
    error_exists = true;
  }

  if (error_exists)
    return false;
  unsigned int frame_number = lookupOrInsertFrameNumber(mapped_transform.child_frame_id_);
  if (getFrame(frame_number)->insertData(TransformStorage(mapped_transform, lookupOrInsertFrameNumber(mapped_transform.frame_id_))))
  {
    frame_authority_[frame_number] = authority;
  }
  else
  {
    ROS_WARN("TF_OLD_DATA ignoring data from the past for frame %s at time %g according to authority %s\nPossible reasons are listed at ", mapped_transform.child_frame_id_.c_str(), mapped_transform.stamp_.toSec(), authority.c_str());
    return false;
  }

  {
    boost::mutex::scoped_lock lock(transforms_changed_mutex_);
    transforms_changed_();
  }

  return true;
};


void Transformer::lookupTransform(const std::string& target_frame, const std::string& source_frame,
                     const ros::Time& time, StampedTransform& transform) const
{
  std::string mapped_target_frame = assert_resolved(tf_prefix_, target_frame);
  std::string mapped_source_frame = assert_resolved(tf_prefix_, source_frame);

  // Short circuit if zero length transform to allow lookups on non existant links
  if (mapped_source_frame == mapped_target_frame)
  {
    transform.setIdentity();

    if (time == ros::Time())
      transform.stamp_ = now();
    else
      transform.stamp_  = time;

    transform.child_frame_id_ = mapped_source_frame;
    transform.frame_id_ = mapped_target_frame;
    return;
  }

  //  printf("Mapped Source: %s \nMapped Target: %s\n", mapped_source_frame.c_str(), mapped_target_frame.c_str());
#ifdef WIN32
  int retval = NO_ERR;
#else
  int retval = NO_ERROR;
#endif
  ros::Time temp_time;
  std::string error_string;
  //If getting the latest get the latest common time
  if (time == ros::Time())
    retval = getLatestCommonTime(mapped_target_frame, mapped_source_frame, temp_time, &error_string);
  else
    temp_time = time;

  TransformLists t_list;

#ifdef WIN32
  if (retval == NO_ERR)
#else
  if (retval == NO_ERROR)
#endif //WIN32
    try
    {
      retval = lookupLists(lookupFrameNumber( mapped_target_frame), temp_time, lookupFrameNumber( mapped_source_frame), t_list, &error_string);
    }
    catch (tf::LookupException &ex)
    {
      error_string = ex.what();
      retval = LOOKUP_ERROR;
    }
#ifdef WIN32
  if (retval != NO_ERR)
#else
  if (retval != NO_ERROR)
#endif //WIN32
  {
    std::stringstream ss;
    ss << " When trying to transform between " << mapped_source_frame << " and " << mapped_target_frame <<".";
    if (retval == LOOKUP_ERROR)
      throw LookupException(error_string + ss.str());
    if (retval == CONNECTIVITY_ERROR)
      throw ConnectivityException(error_string + ss.str());
  }

  if (test_extrapolation(temp_time, t_list, &error_string))
    {
    std::stringstream ss;
    if (time == ros::Time())// Using latest common time if we extrapolate this means that one of the links is out of date
    {
      ss << "Could not find a common time " << mapped_source_frame << " and " << mapped_target_frame <<".";
      throw ConnectivityException(ss.str());
    }
    else
    {
      ss << " When trying to transform between " << mapped_source_frame << " and " << mapped_target_frame <<"."<< std::endl;
      throw ExtrapolationException(error_string + ss.str());
    }
    }


  transform.setData( computeTransformFromList(t_list));
  transform.stamp_ = temp_time;
  transform.frame_id_ = target_frame;
  transform.child_frame_id_ = source_frame;

};


void Transformer::lookupTransform(const std::string& target_frame,const ros::Time& target_time, const std::string& source_frame,
                     const ros::Time& source_time, const std::string& fixed_frame, StampedTransform& transform) const
{
  tf::StampedTransform temp1, temp2;
  lookupTransform(fixed_frame, source_frame, source_time, temp1);
  lookupTransform(target_frame, fixed_frame, target_time, temp2);
  transform.setData( temp2 * temp1);
  transform.stamp_ = temp2.stamp_;
  transform.frame_id_ = target_frame;
  transform.child_frame_id_ = source_frame;

};


void Transformer::lookupTwist(const std::string& tracking_frame, const std::string& observation_frame,
                              const ros::Time& time, const ros::Duration& averaging_interval, 
                              geometry_msgs::Twist& twist) const
{
  lookupTwist(tracking_frame, observation_frame, observation_frame, tf::Point(0,0,0), tracking_frame, time, averaging_interval, twist);
};
// ref point is origin of tracking_frame, ref_frame = obs_frame


void Transformer::lookupTwist(const std::string& tracking_frame, const std::string& observation_frame, const std::string& reference_frame,
                 const tf::Point & reference_point, const std::string& reference_point_frame, 
                 const ros::Time& time, const ros::Duration& averaging_interval, 
                 geometry_msgs::Twist& twist) const
{
  ros::Time latest_time, target_time;
  getLatestCommonTime(observation_frame, tracking_frame, latest_time, NULL); ///\TODO check time on reference point too

  if (ros::Time() == time)
    target_time = latest_time;
  else
    target_time = time;

  ros::Time end_time = std::min(target_time + averaging_interval *0.5 , latest_time);
  
  ros::Time start_time = std::max(ros::Time().fromSec(.00001) + averaging_interval, end_time) - averaging_interval;  // don't collide with zero
  ros::Duration corrected_averaging_interval = end_time - start_time; //correct for the possiblity that start time was truncated above.
  StampedTransform start, end;
  lookupTransform(observation_frame, tracking_frame, start_time, start);
  lookupTransform(observation_frame, tracking_frame, end_time, end);


  btMatrix3x3 temp = start.getBasis().inverse() * end.getBasis();
  btQuaternion quat_temp;
  temp.getRotation(quat_temp);
  btVector3 o = start.getBasis() * quat_temp.getAxis();
  btScalar ang = quat_temp.getAngle();
  
  double delta_x = end.getOrigin().getX() - start.getOrigin().getX();
  double delta_y = end.getOrigin().getY() - start.getOrigin().getY();
  double delta_z = end.getOrigin().getZ() - start.getOrigin().getZ();


  btVector3 twist_vel ((delta_x)/corrected_averaging_interval.toSec(), 
                       (delta_y)/corrected_averaging_interval.toSec(),
                       (delta_z)/corrected_averaging_interval.toSec());
  btVector3 twist_rot = o * (ang / corrected_averaging_interval.toSec());


  // This is a twist w/ reference frame in observation_frame  and reference point is in the tracking_frame at the origin (at start_time)


  //correct for the position of the reference frame
  tf::StampedTransform inverse;
  lookupTransform(reference_frame,tracking_frame,  target_time, inverse);
  btVector3 out_rot = inverse.getBasis() * twist_rot;
  btVector3 out_vel = inverse.getBasis()* twist_vel + inverse.getOrigin().cross(out_rot);


  //Rereference the twist about a new reference point
  // Start by computing the original reference point in the reference frame:
  tf::Stamped<tf::Point> rp_orig(tf::Point(0,0,0), target_time, tracking_frame);
  transformPoint(reference_frame, rp_orig, rp_orig);
  // convert the requrested reference point into the right frame
  tf::Stamped<tf::Point> rp_desired(reference_point, target_time, reference_point_frame);
  transformPoint(reference_frame, rp_desired, rp_desired);
  // compute the delta
  tf::Point delta = rp_desired - rp_orig;
  // Correct for the change in reference point 
  out_vel = out_vel + out_rot * delta;
  // out_rot unchanged   

  /*
    printf("KDL: Rotation %f %f %f, Translation:%f %f %f\n", 
         out_rot.x(),out_rot.y(),out_rot.z(),
         out_vel.x(),out_vel.y(),out_vel.z());
  */   

  twist.linear.x =  out_vel.x();
  twist.linear.y =  out_vel.y();
  twist.linear.z =  out_vel.z();
  twist.angular.x =  out_rot.x();
  twist.angular.y =  out_rot.y();
  twist.angular.z =  out_rot.z();

};

bool Transformer::waitForTransform(const std::string& target_frame, const std::string& source_frame,
                                   const ros::Time& time,
                                   const ros::Duration& timeout, const ros::Duration& polling_sleep_duration,
                                   std::string* error_msg) const
{
  if (!using_dedicated_thread_)
  {
    std::string error_string = "Do not call waitForTransform unless you are using another thread for populating data. Without a dedicated thread it will always timeout.  If you have a seperate thread servicing tf messages, call setUsingDedicatedThread(true)";
    ROS_ERROR("%s",error_string.c_str());
    
    if (error_msg) 
      *error_msg = error_string;
    return false;
  }
  ros::Time start_time = now();
  while (!canTransform(target_frame, source_frame, time, error_msg))
  {
    if ((now() - start_time) >= timeout)
      return false;
#ifdef WIN32
	Sleep(polling_sleep_duration.sec * 1000 + polling_sleep_duration.nsec / 1000000);
#else
    usleep(polling_sleep_duration.sec * 1000000 + polling_sleep_duration.nsec / 1000); //hack to avoid calling ros::Time::now() in Duration.sleep
#endif //WIN32
  }
  return true;
}


bool Transformer::canTransform(const std::string& target_frame, const std::string& source_frame,
                               const ros::Time& time,
                               std::string* error_msg) const
{
  std::string mapped_target_frame = assert_resolved(tf_prefix_, target_frame);
  std::string mapped_source_frame = assert_resolved(tf_prefix_, source_frame);

  ros::Time local_time = time;

  //break out early if no op transform
  if (mapped_source_frame == mapped_target_frame) return true;

#ifdef WIN32
  if (local_time == ros::Time())
    if (NO_ERR != getLatestCommonTime(mapped_source_frame, mapped_target_frame, local_time, error_msg)) // set time if zero
    {
      return false;
    }
#else
  if (local_time == ros::Time())
    if (NO_ERROR != getLatestCommonTime(mapped_source_frame, mapped_target_frame, local_time, error_msg)) // set time if zero
    {
      return false;
    }
#endif //WIN32
  


  TransformLists t_list;
  ///\todo check return
  int retval;
  try
  {
    retval = lookupLists(lookupFrameNumber( mapped_target_frame), local_time, lookupFrameNumber( mapped_source_frame), t_list, error_msg);
  }
  catch (tf::LookupException &ex)
  {
    return false;
  }
  


  ///\todo WRITE HELPER FUNCITON TO RETHROW
#ifdef WIN32
  if (retval != NO_ERR)
#else
  if (retval != NO_ERROR)
#endif //WIIN32
  {
    if (retval == LOOKUP_ERROR)
    {
      return false;
    }
    if (retval == CONNECTIVITY_ERROR)
    {
      return false;
    }
  }

  if (test_extrapolation(local_time, t_list, error_msg))
    {
      return false;
    }

  return true;
};

bool Transformer::canTransform(const std::string& target_frame,const ros::Time& target_time, const std::string& source_frame,
                               const ros::Time& source_time, const std::string& fixed_frame,
                               std::string* error_msg) const
{
  return canTransform(target_frame, fixed_frame, target_time) && canTransform(fixed_frame, source_frame, source_time, error_msg);
};

bool Transformer::waitForTransform(const std::string& target_frame,const ros::Time& target_time, const std::string& source_frame,
                                   const ros::Time& source_time, const std::string& fixed_frame,
                                   const ros::Duration& timeout, const ros::Duration& polling_sleep_duration,
                                   std::string* error_msg) const
{
  return waitForTransform(target_frame, fixed_frame, target_time, timeout, polling_sleep_duration, error_msg) && waitForTransform(fixed_frame, source_frame, source_time, timeout, polling_sleep_duration, error_msg);
};


bool Transformer::getParent(const std::string& frame_id, ros::Time time, std::string& parent) const
{
  std::string mapped_frame_id = assert_resolved(tf_prefix_, frame_id);
  tf::TimeCache* cache;
  try
  {
    cache = getFrame(lookupFrameNumber(mapped_frame_id));
  }
  catch  (tf::LookupException &ex)
  {
    ROS_ERROR("Transformer::getParent: %s",ex.what());
    return false;
  }

  TransformStorage temp;
  if (! cache->getData(time, temp)) {
    ROS_DEBUG("Transformer::getParent: No data for parent of %s", mapped_frame_id.c_str());
    return false;
  }
  if (temp.frame_id_ == "NO_PARENT") {
    ROS_DEBUG("Transformer::getParent: No parent for %s", mapped_frame_id.c_str());
    return false;
  }
  parent= temp.frame_id_;
  return true;

};


bool Transformer::frameExists(const std::string& frame_id_str) const
{
  boost::mutex::scoped_lock(frame_mutex_);
  std::string frame_id_resolveped = assert_resolved(tf_prefix_, frame_id_str);
  
  std::map<std::string, unsigned int>::const_iterator map_it = frameIDs_.find(frame_id_resolveped);
  if (map_it == frameIDs_.end())
  {
      return false;
  }
  else
    return true;
}

void Transformer::setExtrapolationLimit(const ros::Duration& distance)
{
  max_extrapolation_distance_ = distance;
}

int Transformer::getLatestCommonTime(const std::string& source, const std::string& dest, ros::Time & time, std::string * error_string) const
{
  std::string mapped_source = assert_resolved(tf_prefix_, source);
  std::string mapped_dest = assert_resolved(tf_prefix_, dest);

  time = ros::Time(UINT_MAX, 999999999);///\todo replace with ros::TIME_MAX when it is merged from stable
  int retval;
  TransformLists lists;
  try
  {
    retval = lookupLists(lookupFrameNumber(mapped_dest), ros::Time(), lookupFrameNumber(mapped_source), lists, error_string);
  }
  catch (tf::LookupException &ex)
  {
    time = ros::Time();
    if (error_string) *error_string = ex.what();
    return LOOKUP_ERROR;
  }
#ifdef WIN32
  if (retval == NO_ERR)
#else
  if (retval == NO_ERROR)
#endif //WIN32
  {
    //Set time to latest timestamp of frameid in case of target and mapped_source frame id are the same
    if (lists.inverseTransforms.size() == 0 && lists.forwardTransforms.size() == 0)
    {
      time = now();
      return retval;
    }

    for (unsigned int i = 0; i < lists.inverseTransforms.size(); i++)
    {
      if (time > lists.inverseTransforms[i].stamp_)
        time = lists.inverseTransforms[i].stamp_;
    }
    for (unsigned int i = 0; i < lists.forwardTransforms.size(); i++)
    {
      if (time > lists.forwardTransforms[i].stamp_)
        time = lists.forwardTransforms[i].stamp_;
    }

  }
  else
    time.fromSec(0);

  return retval;
};



int Transformer::lookupLists(unsigned int target_frame, ros::Time time, unsigned int source_frame, TransformLists& lists, std::string * error_string) const
{
  /*  timeval tempt;
  gettimeofday(&tempt,NULL);
  std::cerr << "Looking up list at " <<tempt.tv_sec * 1000000ULL + tempt.tv_usec << std::endl;
  */

  ///\todo add fixed frame support

  //Clear lists before operating
  lists.forwardTransforms.clear();
  lists.inverseTransforms.clear();
  //  TransformLists mTfLs;
  if (target_frame == source_frame)
    return 0;  //Don't do anythign if we're not going anywhere

  TransformStorage temp;

  unsigned int frame = source_frame;
  unsigned int counter = 0;  //A counter to keep track of how deep we've descended
  unsigned int last_inverse;
  if (getFrame(frame) == NULL) //Test if source frame exists this will throw a lookup error if it does not (inside the loop it will be caught)
  {
    if (error_string) *error_string = "Source frame '"+lookupFrameString(frame)+"' does not exist is tf tree.";
    return LOOKUP_ERROR;//throw LookupException("Frame didn't exist");
  }
  while (true)
    {
      //      printf("getting data from %d:%s \n", frame, lookupFrameString(frame).c_str());

      TimeCache* pointer = getFrame(frame);
      ROS_ASSERT(pointer);

      if (! pointer->getData(time, temp))
      {
        last_inverse = frame;
        // this is thrown when there is no data
        break;
      }

      //break if parent is NO_PARENT (0)
      if (frame == 0)
      {
        last_inverse = frame;
        break;
      }
      lists.inverseTransforms.push_back(temp);

      frame = temp.frame_id_num_;


      /* Check if we've gone too deep.  A loop in the tree would cause this */
      if (counter++ > MAX_GRAPH_DEPTH)
      {
        if (error_string)
        {
          std::stringstream ss;
          ss<<"The tf tree is invalid because it contains a loop." << std::endl
            << allFramesAsString() << std::endl;
          *error_string =ss.str();
        }
        return LOOKUP_ERROR;
        //        throw(LookupException(ss.str()));
      }
    }
  /*
    timeval tempt2;
  gettimeofday(&tempt2,NULL);
  std::cerr << "Side A " <<tempt.tv_sec * 1000000LL + tempt.tv_usec- tempt2.tv_sec * 1000000LL - tempt2.tv_usec << std::endl;
  */
  frame = target_frame;
  counter = 0;
  unsigned int last_forward;
  if (getFrame(frame) == NULL)
  {
    if (error_string) *error_string = "Target frame '"+lookupFrameString(frame)+"' does not exist is tf tree.";
    return LOOKUP_ERROR;
  }//throw LookupException("fixme");; //Test if source frame exists this will throw a lookup error if it does not (inside the loop it will be caught)
  while (true)
    {

      TimeCache* pointer = getFrame(frame);
      ROS_ASSERT(pointer);


      if(!  pointer->getData(time, temp))
      {
        last_forward = frame;
        break;
      }

      //break if parent is NO_PARENT (0)
      if (frame == 0)
      {
        last_forward = frame;
        break;
      }
      //      std::cout << "pushing back" << temp.frame_id_ << std::endl;
      lists.forwardTransforms.push_back(temp);
      frame = temp.frame_id_num_;

      /* Check if we've gone too deep.  A loop in the tree would cause this*/
      if (counter++ > MAX_GRAPH_DEPTH){
        if (error_string)
        {
          std::stringstream ss;
          ss<<"The tf tree is invalid because it contains a loop." << std::endl
            << allFramesAsString() << std::endl;
          *error_string = ss.str();
        }
        return LOOKUP_ERROR;//throw(LookupException(ss.str()));
      }
    }
  /*
  gettimeofday(&tempt2,NULL);
  std::cerr << "Side B " <<tempt.tv_sec * 1000000LL + tempt.tv_usec- tempt2.tv_sec * 1000000LL - tempt2.tv_usec << std::endl;
  */

  std::string connectivity_error("Could not find a connection between '"+lookupFrameString(target_frame)+"' and '"+
                                 lookupFrameString(source_frame)+"' because they are not part of the same tree."+
                                 "Tf has two or more unconnected trees.");
  /* Check the zero length cases*/
  if (lists.inverseTransforms.size() == 0)
  {
    if (lists.forwardTransforms.size() == 0) //If it's going to itself it's already been caught
    {
      if (error_string) *error_string = connectivity_error;
      return CONNECTIVITY_ERROR;
    }

    if (last_forward != source_frame)  //\todo match with case A
    {
      if (error_string) *error_string = connectivity_error;
      return CONNECTIVITY_ERROR;
    }
    else return 0;
  }

  if (lists.forwardTransforms.size() == 0)
  {
    if (lists.inverseTransforms.size() == 0)  //If it's going to itself it's already been caught
    {//\todo remove THis is the same as case D
      if (error_string) *error_string = connectivity_error;
      return CONNECTIVITY_ERROR;
    }

    try
    {
#ifdef WIN32
	  if (lookupFrameNumber(lists.inverseTransforms[lists.inverseTransforms.size() - 1 ].frame_id_) != target_frame)
#else
      if (lookupFrameNumber(lists.inverseTransforms.back().frame_id_) != target_frame)
#endif //WIN32
      {
        if (error_string) *error_string = connectivity_error;
        return CONNECTIVITY_ERROR;
    }
    else return 0;
    }
    catch (tf::LookupException & ex)
    {
      if (error_string) *error_string = ex.what();
      return LOOKUP_ERROR;
    }
  }


  /* Make sure the end of the search shares a parent. */
  if (last_forward != last_inverse)
  {
    if (error_string) *error_string = connectivity_error;
    return CONNECTIVITY_ERROR;
  }
  /* Make sure that we don't have a no parent at the top */
  try
  {
#ifdef WIN32
	if (lookupFrameNumber(lists.inverseTransforms[lists.inverseTransforms.size() - 1 ].child_frame_id_) == 0 || lookupFrameNumber( lists.forwardTransforms[lists.forwardTransforms.size() - 1 ].child_frame_id_) == 0)
#else
    if (lookupFrameNumber(lists.inverseTransforms.back().child_frame_id_) == 0 || lookupFrameNumber( lists.forwardTransforms.back().child_frame_id_) == 0)
#endif //WIN32
    {
      //if (error_string) *error_string = "NO_PARENT at top of tree";
      if (error_string) *error_string = connectivity_error;
      return CONNECTIVITY_ERROR;
    }

    /*
      gettimeofday(&tempt2,NULL);
      std::cerr << "Base Cases done" <<tempt.tv_sec * 1000000LL + tempt.tv_usec- tempt2.tv_sec * 1000000LL - tempt2.tv_usec << std::endl;
    */
#ifdef WIN32
    while (lookupFrameNumber(lists.inverseTransforms[lists.inverseTransforms.size() - 1 ].child_frame_id_) == lookupFrameNumber(lists.forwardTransforms[lists.forwardTransforms.size() - 1 ].child_frame_id_))
#else
    while (lookupFrameNumber(lists.inverseTransforms.back().child_frame_id_) == lookupFrameNumber(lists.forwardTransforms.back().child_frame_id_))
#endif //WIN32
    {
      lists.inverseTransforms.pop_back();
      lists.forwardTransforms.pop_back();

      // Make sure we don't go beyond the beginning of the list.
      // (The while statement above doesn't fail if you hit the beginning of the list,
      // which happens in the zero distance case.)
      if (lists.inverseTransforms.size() == 0 || lists.forwardTransforms.size() == 0)
	break;
    }
  }
  catch (tf::LookupException & ex)
  {
    if (error_string) *error_string = ex.what();
    return LOOKUP_ERROR;
  }  /*
       gettimeofday(&tempt2,NULL);
       std::cerr << "Done looking up list " <<tempt.tv_sec * 1000000LL + tempt.tv_usec- tempt2.tv_sec * 1000000LL - tempt2.tv_usec << std::endl;
     */
  return 0;

  }


bool Transformer::test_extrapolation_one_value(const ros::Time& target_time, const TransformStorage& tr, std::string* error_string) const
{
  std::stringstream ss;
  ss << std::fixed;
  ss.precision(3);

  if (tr.mode_ == ONE_VALUE)
  {
    if (tr.stamp_ - target_time > max_extrapolation_distance_ || target_time - tr.stamp_ > max_extrapolation_distance_)
    {
      if (error_string) {
        ss << "You requested a transform at time " << (target_time).toSec() 
           << ",\n but the tf buffer only contains a single transform " 
           << "at time " << tr.stamp_.toSec() << ".\n";
        if ( max_extrapolation_distance_ > ros::Duration(0))
        {
          ss << "The tf extrapollation distance is set to " 
             << (max_extrapolation_distance_).toSec() <<" seconds.\n";
        }
        *error_string = ss.str();
      }
      return true;
    }
  }
  return false;
}


bool Transformer::test_extrapolation_past(const ros::Time& target_time, const TransformStorage& tr, std::string* error_string) const
{
  std::stringstream ss;
  ss << std::fixed;
  ss.precision(3);

  if (tr.mode_ == EXTRAPOLATE_BACK &&  tr.stamp_ - target_time > max_extrapolation_distance_)
  {
    if (error_string) {
      ss << "You requested a transform that is " << (now() - target_time).toSec() << " seconds in the past, \n"
         << "but the tf buffer only has a history of " << (now() - tr.stamp_).toSec()  << " seconds.\n";
      if ( max_extrapolation_distance_ > ros::Duration(0))
      {
        ss << "The tf extrapollation distance is set to " 
           << (max_extrapolation_distance_).toSec() <<" seconds.\n";
      }
      *error_string = ss.str();
    }
    return true;
  }
  return false;
}


bool Transformer::test_extrapolation_future(const ros::Time& target_time, const TransformStorage& tr, std::string* error_string) const
{
  std::stringstream ss;
  ss << std::fixed;
  ss.precision(3);

  if( tr.mode_ == EXTRAPOLATE_FORWARD && target_time - tr.stamp_ > max_extrapolation_distance_)
  {
    if (error_string){
      ss << "You requested a transform that is " << (now() - target_time).toSec()*1000 << " miliseconds in the past, \n"
         << "but the most recent transform in the tf buffer is " << (now() - tr.stamp_).toSec()*1000 << " miliseconds old.\n";
      if ( max_extrapolation_distance_ > ros::Duration(0))
      {
        ss << "The tf extrapollation distance is set to " 
           << (max_extrapolation_distance_).toSec() <<" seconds.\n";
      }
      *error_string = ss.str();
    }
    return true;
  }
  return false;
}


bool Transformer::test_extrapolation(const ros::Time& target_time, const TransformLists& lists, std::string * error_string) const
{
  std::stringstream ss;
  ss << std::fixed;
  ss.precision(3);
  for (unsigned int i = 0; i < lists.inverseTransforms.size(); i++)
  {
    if (test_extrapolation_one_value(target_time, lists.inverseTransforms[i], error_string)) return true;
    if (test_extrapolation_past(target_time, lists.inverseTransforms[i], error_string)) return true;
    if (test_extrapolation_future(target_time, lists.inverseTransforms[i], error_string)) return true;
  }

  for (unsigned int i = 0; i < lists.forwardTransforms.size(); i++)
  {
    if (test_extrapolation_one_value(target_time, lists.forwardTransforms[i], error_string)) return true;
    if (test_extrapolation_past(target_time, lists.forwardTransforms[i], error_string)) return true;
    if (test_extrapolation_future(target_time, lists.forwardTransforms[i], error_string)) return true;
  }

  return false;
}


btTransform Transformer::computeTransformFromList(const TransformLists & lists) const
{
  btTransform retTrans;
  retTrans.setIdentity();
  ///@todo change these to iterators
  for (unsigned int i = 0; i < lists.inverseTransforms.size(); i++)
    {
      retTrans *= (lists.inverseTransforms[lists.inverseTransforms.size() -1 - i]); //Reverse to get left multiply
    }
  for (unsigned int i = 0; i < lists.forwardTransforms.size(); i++)
    {
      retTrans = (lists.forwardTransforms[lists.forwardTransforms.size() -1 - i]).inverse() * retTrans; //Do this list backwards(from backwards) for it was generated traveling the wrong way
    }

  return retTrans;
}


std::string Transformer::chainAsString(const std::string & target_frame, ros::Time target_time, const std::string & source_frame, ros::Time source_time, const std::string& fixed_frame) const
{
  std::string error_string;
  std::stringstream mstream;
  TransformLists lists;
  ///\todo check return code
  try
  {
    lookupLists(lookupFrameNumber(target_frame), target_time, lookupFrameNumber(source_frame), lists, &error_string);
  }
  catch (tf::LookupException &ex)
  {
    mstream << ex.what();
    return mstream.str();
  }
  mstream << "Inverse Transforms:" <<std::endl;
  for (unsigned int i = 0; i < lists.inverseTransforms.size(); i++)
    {
      mstream << lists.inverseTransforms[i].child_frame_id_<<", ";
    }
  mstream << std::endl;

  mstream << "Forward Transforms: "<<std::endl ;
  for (unsigned int i = 0; i < lists.forwardTransforms.size(); i++)
    {
      mstream << lists.forwardTransforms[i].child_frame_id_<<", ";
    }
  mstream << std::endl;
  return mstream.str();
}

void Transformer::chainAsVector(const std::string & target_frame, ros::Time target_time, const std::string & source_frame, ros::Time source_time, const std::string& fixed_frame, std::vector<std::string>& output) const
{
  std::string error_string;
  TransformLists lists;
  ///\todo check return code
  try
  {
    lookupLists(lookupFrameNumber(target_frame), target_time, lookupFrameNumber(source_frame), lists, &error_string);
  }
  catch (tf::LookupException &ex)
  {
    return;
  }

  output.clear(); //empty vector
#ifdef WIN32
  if (lists.inverseTransforms.size() != 0 )
#else
  if (! lists.inverseTransforms.empty())
#endif //WIN32
  {
    for (unsigned int i = 0; i < lists.inverseTransforms.size(); i++)
    {
      output.push_back(lists.inverseTransforms[i].child_frame_id_);
    }
#ifdef WIN32
	output.push_back(lists.inverseTransforms[lists.inverseTransforms.size() - 1].frame_id_);
#else
    output.push_back(lists.inverseTransforms.back().frame_id_);
#endif //WIN32

    for (unsigned int i = 0; i < lists.forwardTransforms.size(); i++)
    {
      output.push_back(lists.forwardTransforms[i].child_frame_id_);
    }
  }
  else
  {
    output.push_back(source_frame);
    
    for (unsigned int i = 0; i < lists.forwardTransforms.size(); i++)
    {
      output.push_back(lists.forwardTransforms[i].child_frame_id_);
    }
    

  }
}

std::string Transformer::allFramesAsString() const
{
  std::stringstream mstream;
  boost::mutex::scoped_lock(frame_mutex_);

  TransformStorage temp;



  //  for (std::vector< TimeCache*>::iterator  it = frames_.begin(); it != frames_.end(); ++it)
  for (unsigned int counter = 1; counter < frames_.size(); counter ++)
  {
    unsigned int frame_id_num;
    if(  getFrame(counter)->getData(ros::Time(), temp))
      frame_id_num = temp.frame_id_num_;
    else
    {
      frame_id_num = 0;
    }
    mstream << "Frame "<< frameIDs_reverse[counter] << " exists with parent " << frameIDs_reverse[frame_id_num] << "." <<std::endl;
  }
  return mstream.str();
}

std::string Transformer::allFramesAsDot() const
{
  std::stringstream mstream;
  mstream << "digraph G {" << std::endl;
  boost::mutex::scoped_lock(frame_mutex_);

  TransformStorage temp;

  ros::Time current_time = now();

  if (frames_.size() ==1)
    mstream <<"\"no tf data recieved\"";

  mstream.precision(3);
  mstream.setf(std::ios::fixed,std::ios::floatfield);
    
   //  for (std::vector< TimeCache*>::iterator  it = frames_.begin(); it != frames_.end(); ++it)
  for (unsigned int counter = 1; counter < frames_.size(); counter ++)//one referenced for 0 is no frame
  {
    unsigned int frame_id_num;
    if(  getFrame(counter)->getData(ros::Time(), temp))
      frame_id_num = temp.frame_id_num_;
    else
    {
      frame_id_num = 0;
    }
    if (frame_id_num != 0)
    {
      std::string authority = "no recorded authority";
      std::map<unsigned int, std::string>::const_iterator it = frame_authority_.find(counter);
      if (it != frame_authority_.end())
        authority = it->second;

      double rate = getFrame(counter)->getListLength() / std::max((getFrame(counter)->getLatestTimestamp().toSec() -
                                                                   getFrame(counter)->getOldestTimestamp().toSec() ), 0.0001);

      mstream << std::fixed; //fixed point notation
      mstream.precision(3); //3 decimal places
      mstream << "\"" << frameIDs_reverse[frame_id_num] << "\"" << " -> "
              << "\"" << frameIDs_reverse[counter] << "\"" << "[label=\""
        //<< "Time: " << current_time.toSec() << "\\n"
              << "Broadcaster: " << authority << "\\n"
              << "Average rate: " << rate << " Hz\\n"
              << "Most recent transform: " << (current_time - getFrame(counter)->getLatestTimestamp()).toSec() << " sec old \\n"
        //    << "(time: " << getFrame(counter)->getLatestTimestamp().toSec() << ")\\n"
        //    << "Oldest transform: " << (current_time - getFrame(counter)->getOldestTimestamp()).toSec() << " sec old \\n"
        //    << "(time: " << (getFrame(counter)->getOldestTimestamp()).toSec() << ")\\n"
              << "Buffer length: " << (getFrame(counter)->getLatestTimestamp()-getFrame(counter)->getOldestTimestamp()).toSec() << " sec\\n"
              <<"\"];" <<std::endl;
    }
  }
  
  for (unsigned int counter = 1; counter < frames_.size(); counter ++)//one referenced for 0 is no frame
  {
    unsigned int frame_id_num;
    if(  getFrame(counter)->getData(ros::Time(), temp))
      frame_id_num = temp.frame_id_num_;
    else
      {
	frame_id_num = 0;
      }

    if(frameIDs_reverse[frame_id_num]=="NO_PARENT")
    {
      mstream << "edge [style=invis];" <<std::endl;
      mstream << " subgraph cluster_legend { style=bold; color=black; label =\"view_frames Result\";\n"
              << "\"Recorded at time: " << current_time.toSec() << "\"[ shape=plaintext ] ;\n "
	      << "}" << "->" << "\"" << frameIDs_reverse[counter]<<"\";" <<std::endl;
    }
  }
  mstream << "}";
  return mstream.str();
}

void Transformer::getFrameStrings(std::vector<std::string> & vec) const
{
  vec.clear();

  boost::mutex::scoped_lock(frame_mutex_);

  TransformStorage temp;

  //  for (std::vector< TimeCache*>::iterator  it = frames_.begin(); it != frames_.end(); ++it)
  for (unsigned int counter = 1; counter < frames_.size(); counter ++)
  {
    vec.push_back(frameIDs_reverse[counter]);
  }
  return;
}

tf::TimeCache* Transformer::getFrame(unsigned int frame_id) const
{
  if (frame_id == 0) /// @todo check larger values too
    return NULL;
  else
    return frames_[frame_id];
};


void Transformer::transformQuaternion(const std::string& target_frame, const Stamped<Quaternion>& stamped_in, Stamped<Quaternion>& stamped_out) const
{
  tf::assertQuaternionValid(stamped_in);

  StampedTransform transform;
  lookupTransform(target_frame, stamped_in.frame_id_, stamped_in.stamp_, transform);

  stamped_out.setData( transform * stamped_in);
  stamped_out.stamp_ = transform.stamp_;
  stamped_out.frame_id_ = target_frame;
};


void Transformer::transformVector(const std::string& target_frame,
                                  const Stamped<tf::Vector3>& stamped_in,
                                  Stamped<tf::Vector3>& stamped_out) const
{
  StampedTransform transform;
  lookupTransform(target_frame, stamped_in.frame_id_, stamped_in.stamp_, transform);

  /** \todo may not be most efficient */
  btVector3 end = stamped_in;
  btVector3 origin = btVector3(0,0,0);
  btVector3 output = (transform * end) - (transform * origin);
  stamped_out.setData( output);

  stamped_out.stamp_ = transform.stamp_;
  stamped_out.frame_id_ = target_frame;
};


void Transformer::transformPoint(const std::string& target_frame, const Stamped<Point>& stamped_in, Stamped<Point>& stamped_out) const
{
  StampedTransform transform;
  lookupTransform(target_frame, stamped_in.frame_id_, stamped_in.stamp_, transform);

  stamped_out.setData(transform * stamped_in);
  stamped_out.stamp_ = transform.stamp_;
  stamped_out.frame_id_ = target_frame;
};

void Transformer::transformPose(const std::string& target_frame, const Stamped<Pose>& stamped_in, Stamped<Pose>& stamped_out) const
{
  StampedTransform transform;
  lookupTransform(target_frame, stamped_in.frame_id_, stamped_in.stamp_, transform);

  stamped_out.setData(transform * stamped_in);
  stamped_out.stamp_ = transform.stamp_;
  stamped_out.frame_id_ = target_frame;
};


void Transformer::transformQuaternion(const std::string& target_frame, const ros::Time& target_time,
                                      const Stamped<Quaternion>& stamped_in,
                                      const std::string& fixed_frame,
                                      Stamped<Quaternion>& stamped_out) const
{
  tf::assertQuaternionValid(stamped_in);
  StampedTransform transform;
  lookupTransform(target_frame, target_time,
                  stamped_in.frame_id_,stamped_in.stamp_,
                  fixed_frame, transform);

  stamped_out.setData( transform * stamped_in);
  stamped_out.stamp_ = transform.stamp_;
  stamped_out.frame_id_ = target_frame;
};


void Transformer::transformVector(const std::string& target_frame, const ros::Time& target_time,
                                  const Stamped<Vector3>& stamped_in,
                                  const std::string& fixed_frame,
                                  Stamped<Vector3>& stamped_out) const
{
  StampedTransform transform;
  lookupTransform(target_frame, target_time,
                  stamped_in.frame_id_,stamped_in.stamp_,
                  fixed_frame, transform);

  /** \todo may not be most efficient */
  btVector3 end = stamped_in;
  btVector3 origin = btVector3(0,0,0);
  btVector3 output = (transform * end) - (transform * origin);
  stamped_out.setData( output);

  stamped_out.stamp_ = transform.stamp_;
  stamped_out.frame_id_ = target_frame;
};


void Transformer::transformPoint(const std::string& target_frame, const ros::Time& target_time,
                                 const Stamped<Point>& stamped_in,
                                 const std::string& fixed_frame,
                                 Stamped<Point>& stamped_out) const
{
  StampedTransform transform;
  lookupTransform(target_frame, target_time,
                  stamped_in.frame_id_,stamped_in.stamp_,
                  fixed_frame, transform);

  stamped_out.setData(transform * stamped_in);
  stamped_out.stamp_ = transform.stamp_;
  stamped_out.frame_id_ = target_frame;
};

void Transformer::transformPose(const std::string& target_frame, const ros::Time& target_time,
                                const Stamped<Pose>& stamped_in,
                                const std::string& fixed_frame,
                                Stamped<Pose>& stamped_out) const
{
  StampedTransform transform;
  lookupTransform(target_frame, target_time,
                  stamped_in.frame_id_,stamped_in.stamp_,
                  fixed_frame, transform);

  stamped_out.setData(transform * stamped_in);
  stamped_out.stamp_ = transform.stamp_;
  stamped_out.frame_id_ = target_frame;
};

boost::signals::connection Transformer::addTransformsChangedListener(boost::function<void(void)> callback)
{
  boost::mutex::scoped_lock lock(transforms_changed_mutex_);
  return transforms_changed_.connect(callback);
}

void Transformer::removeTransformsChangedListener(boost::signals::connection c)
{
  boost::mutex::scoped_lock lock(transforms_changed_mutex_);
  c.disconnect();
}
