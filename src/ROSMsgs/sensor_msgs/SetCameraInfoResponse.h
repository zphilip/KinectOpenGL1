/* Software License Agreement (BSD License)
 *
 * Copyright (c) 2011, Willow Garage, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *  * Neither the name of Willow Garage, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Auto-generated by genmsg_cpp from file F:\ROS\work\ws\sensor_msgs\srv\SetCameraInfo.srv
 *
 */


#ifndef SENSOR_MSGS_MESSAGE_SETCAMERAINFORESPONSE_H
#define SENSOR_MSGS_MESSAGE_SETCAMERAINFORESPONSE_H


#include <string>
#include <vector>
#include <map>

#include <ros/types.h>
#include <ros/serialization.h>
#include <ros/builtin_message_traits.h>
#include <ros/message_operations.h>


namespace sensor_msgs
{
template <class ContainerAllocator>
struct SetCameraInfoResponse_
{
  typedef SetCameraInfoResponse_<ContainerAllocator> Type;

  SetCameraInfoResponse_()
    : success(false)
    , status_message()  {
    }
  SetCameraInfoResponse_(const ContainerAllocator& _alloc)
    : success(false)
    , status_message(_alloc)  {
    }



   typedef uint8_t _success_type;
  _success_type success;

   typedef std::basic_string<char, std::char_traits<char>, typename ContainerAllocator::template rebind<char>::other >  _status_message_type;
  _status_message_type status_message;




  typedef boost::shared_ptr< ::sensor_msgs::SetCameraInfoResponse_<ContainerAllocator> > Ptr;
  typedef boost::shared_ptr< ::sensor_msgs::SetCameraInfoResponse_<ContainerAllocator> const> ConstPtr;
  boost::shared_ptr<std::map<std::string, std::string> > __connection_header;

}; // struct SetCameraInfoResponse_

typedef ::sensor_msgs::SetCameraInfoResponse_<std::allocator<void> > SetCameraInfoResponse;

typedef boost::shared_ptr< ::sensor_msgs::SetCameraInfoResponse > SetCameraInfoResponsePtr;
typedef boost::shared_ptr< ::sensor_msgs::SetCameraInfoResponse const> SetCameraInfoResponseConstPtr;

// constants requiring out of line definition



template<typename ContainerAllocator>
std::ostream& operator<<(std::ostream& s, const ::sensor_msgs::SetCameraInfoResponse_<ContainerAllocator> & v)
{
ros::message_operations::Printer< ::sensor_msgs::SetCameraInfoResponse_<ContainerAllocator> >::stream(s, "", v);
return s;
}

} // namespace sensor_msgs

namespace ros
{
namespace message_traits
{



// BOOLTRAITS {'IsFixedSize': False, 'IsMessage': True, 'HasHeader': False}
// {'std_msgs': ['F:/ROS/work/ws/std_msgs/msg'], 'sensor_msgs': ['F:/ROS/work/ws/sensor_msgs/msg', 'F:/ROS/work/ws/sensor_msgs/msg'], 'geometry_msgs': ['F:/ROS/work/ws/geometry_msgs/msg']}

// !!!!!!!!!!! ['__class__', '__delattr__', '__dict__', '__doc__', '__eq__', '__format__', '__getattribute__', '__hash__', '__init__', '__module__', '__ne__', '__new__', '__reduce__', '__reduce_ex__', '__repr__', '__setattr__', '__sizeof__', '__str__', '__subclasshook__', '__weakref__', '_parsed_fields', 'constants', 'fields', 'full_name', 'has_header', 'header_present', 'names', 'package', 'parsed_fields', 'short_name', 'text', 'types']




template <class ContainerAllocator>
struct IsFixedSize< ::sensor_msgs::SetCameraInfoResponse_<ContainerAllocator> >
  : FalseType
  { };

template <class ContainerAllocator>
struct IsFixedSize< ::sensor_msgs::SetCameraInfoResponse_<ContainerAllocator> const>
  : FalseType
  { };

template <class ContainerAllocator>
struct IsMessage< ::sensor_msgs::SetCameraInfoResponse_<ContainerAllocator> >
  : TrueType
  { };

template <class ContainerAllocator>
struct IsMessage< ::sensor_msgs::SetCameraInfoResponse_<ContainerAllocator> const>
  : TrueType
  { };

template <class ContainerAllocator>
struct HasHeader< ::sensor_msgs::SetCameraInfoResponse_<ContainerAllocator> >
  : FalseType
  { };

template <class ContainerAllocator>
struct HasHeader< ::sensor_msgs::SetCameraInfoResponse_<ContainerAllocator> const>
  : FalseType
  { };


template<class ContainerAllocator>
struct MD5Sum< ::sensor_msgs::SetCameraInfoResponse_<ContainerAllocator> >
{
  static const char* value()
  {
    return "2ec6f3eff0161f4257b808b12bc830c2";
  }

  static const char* value(const ::sensor_msgs::SetCameraInfoResponse_<ContainerAllocator>&) { return value(); }
  static const uint64_t static_value1 = 0x2ec6f3eff0161f42ULL;
  static const uint64_t static_value2 = 0x57b808b12bc830c2ULL;
};

template<class ContainerAllocator>
struct DataType< ::sensor_msgs::SetCameraInfoResponse_<ContainerAllocator> >
{
  static const char* value()
  {
    return "sensor_msgs/SetCameraInfoResponse";
  }

  static const char* value(const ::sensor_msgs::SetCameraInfoResponse_<ContainerAllocator>&) { return value(); }
};

template<class ContainerAllocator>
struct Definition< ::sensor_msgs::SetCameraInfoResponse_<ContainerAllocator> >
{
  static const char* value()
  {
    return "bool success\n\
string status_message\n\
\n\
\n\
";
  }

  static const char* value(const ::sensor_msgs::SetCameraInfoResponse_<ContainerAllocator>&) { return value(); }
};

} // namespace message_traits
} // namespace ros

namespace ros
{
namespace serialization
{

  template<class ContainerAllocator> struct Serializer< ::sensor_msgs::SetCameraInfoResponse_<ContainerAllocator> >
  {
    template<typename Stream, typename T> inline static void allInOne(Stream& stream, T m)
    {
      stream.next(m.success);
      stream.next(m.status_message);
    }

    ROS_DECLARE_ALLINONE_SERIALIZER;
  }; // struct SetCameraInfoResponse_

} // namespace serialization
} // namespace ros

namespace ros
{
namespace message_operations
{

template<class ContainerAllocator>
struct Printer< ::sensor_msgs::SetCameraInfoResponse_<ContainerAllocator> >
{
  template<typename Stream> static void stream(Stream& s, const std::string& indent, const ::sensor_msgs::SetCameraInfoResponse_<ContainerAllocator>& v)
  {
    s << indent << "success: ";
    Printer<uint8_t>::stream(s, indent + "  ", v.success);
    s << indent << "status_message: ";
    Printer<std::basic_string<char, std::char_traits<char>, typename ContainerAllocator::template rebind<char>::other > >::stream(s, indent + "  ", v.status_message);
  }
};

} // namespace message_operations
} // namespace ros

#endif // SENSOR_MSGS_MESSAGE_SETCAMERAINFORESPONSE_H
