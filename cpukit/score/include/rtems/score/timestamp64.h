/**
 *  @file  rtems/score/timestamp64.h
 *
 *  @brief Helpers for Manipulating 64-bit Integer Timestamps
 *
 *  This include file contains helpers for manipulating
 *  64-bit integer timestamps.
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_TIMESTAMP64_H
#define _RTEMS_SCORE_TIMESTAMP64_H

/**
 *  @defgroup SuperCoreTimestamp64 SuperCore Sixty-Four Bit Timestamps
 *
 *  @ingroup Score
 *
 *  This handler encapsulates functionality related to manipulating
 *  the 64 bit integer implementation of SuperCore Timestamps.
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This .h file is not for general use.  It is an alternative
 *  implementation of Timestamps and should only be used that way.
 */
#ifndef _RTEMS_SCORE_TIMESTAMP_H
  #error "Should only be included by rtems/score/timestamp.h"
#endif

/*
 *  Verify something is defined.
 */
#if CPU_TIMESTAMP_USE_INT64 != TRUE && CPU_TIMESTAMP_USE_INT64_INLINE != TRUE
  #error "SuperCore Timestamp64 implementation included but not defined."
#endif

/**
 *   Define the Timestamp control type.
 */
typedef int64_t Timestamp64_Control;

static inline void _Timestamp64_implementation_Set(
  Timestamp64_Control *_time,
  Timestamp64_Control  _seconds,
  Timestamp64_Control  _nanoseconds
)
{
  *_time = _seconds * 1000000000L + _nanoseconds;
}

/**
 *  @brief Set 64-bit timestamp to seconds nanosecond.
 *
 *  This method sets the timestamp to the specified seconds and nanoseconds
 *  value.
 *
 *  @param[in] _time points to the timestamp instance to validate.
 *  @param[in] _seconds is the seconds portion of the timestamp
 *  @param[in] _nanoseconds is the nanoseconds portion of the timestamp
 */
#if CPU_TIMESTAMP_USE_INT64_INLINE == TRUE
  #define _Timestamp64_Set( _time, _seconds, _nanoseconds ) \
    _Timestamp64_implementation_Set( _time, _seconds, _nanoseconds )
#else
  void _Timestamp64_Set(
    Timestamp64_Control *_time,
    Timestamp64_Control  _seconds,
    Timestamp64_Control  _nanoseconds
  );
#endif

static inline void _Timestamp64_implementation_Set_to_zero(
  Timestamp64_Control *_time
)
{
  *_time = 0;
}

/**
 *  @brief Sets the 64-bit timestamp to zero.
 *
 *  This method sets the timestamp to zero value.
 *
 *  @param[in] _time points to the timestamp instance to zero.
 */
#if CPU_TIMESTAMP_USE_INT64_INLINE == TRUE
  #define _Timestamp64_Set_to_zero( _time ) \
    _Timestamp64_implementation_Set_to_zero( _time )
#else
  void _Timestamp64_Set_to_zero(
    Timestamp64_Control *_time
  );
#endif

/**
 *  @brief Determines the validity of a 64-bit timestamp.
 *
 *  This method determines the validity of a timestamp.
 *
 *  @param[in] _time points to the timestamp instance to validate.
 *
 *  @retval This method returns true if @a time is valid and
 *          false otherwise.
 */
#define _Timestamp64_Is_valid( _time ) \
  (1)

static inline bool _Timestamp64_implementation_Less_than(
  const Timestamp64_Control *_lhs,
  const Timestamp64_Control *_rhs
)
{
  return *_lhs < *_rhs;
}

/**
 *  @brief The "less than" operator for 64-bit timestamps.
 *
 *  This method is the less than operator for timestamps.
 *
 *  @param[in] _lhs points to the left hand side timestamp
 *  @param[in] _rhs points to the right hand side timestamp
 *
 *  @retval This method returns true if @a _lhs is less than the @a _rhs and
 *          false otherwise.
 */
#if CPU_TIMESTAMP_USE_INT64_INLINE == TRUE
  #define _Timestamp64_Less_than( _lhs, _rhs ) \
    _Timestamp64_implementation_Less_than( _lhs, _rhs )
#else
  bool _Timestamp64_Less_than(
    const Timestamp64_Control *_lhs,
    const Timestamp64_Control *_rhs
  );
#endif

static inline bool _Timestamp64_implementation_Equal_to(
  const Timestamp64_Control *_lhs,
  const Timestamp64_Control *_rhs
)
{
  return *_lhs == *_rhs;
}

#define _Timestamp64_Greater_than( _lhs, _rhs ) \
  _Timestamp64_Less_than( _rhs, _lhs )

/**
 *  @brief The "equal to" operator for 64-bit timestamps.
 *
 *  This method is the is equal to than operator for timestamps.
 *
 *  @param[in] _lhs points to the left hand side timestamp
 *  @param[in] _rhs points to the right hand side timestamp
 *
 *  @retval This method returns true if @a _lhs is equal to  @a _rhs and
 *          false otherwise.
 */
#if CPU_TIMESTAMP_USE_INT64_INLINE == TRUE
  #define _Timestamp64_Equal_to( _lhs, _rhs ) \
    _Timestamp64_implementation_Equal_to( _lhs, _rhs )
#else
  bool _Timestamp64_Equal_to(
    const Timestamp64_Control *_lhs,
    const Timestamp64_Control *_rhs
  );
#endif

static inline void _Timestamp64_implementation_Add_to(
  Timestamp64_Control       *_time,
  const Timestamp64_Control *_add
)
{
  *_time += *_add;
}

/**
 *  @brief Add two 64-bit timestamps.
 *
 *  This routine adds two timestamps.  The second argument is added
 *  to the first.
 *
 *  @param[in] _time points to the base time to be added to
 *  @param[in] _add points to the timestamp to add to the first argument
 *
 *  @retval This method returns the number of seconds @a time increased by.
 */
#if CPU_TIMESTAMP_USE_INT64_INLINE == TRUE
  #define _Timestamp64_Add_to( _time, _add ) \
    _Timestamp64_implementation_Add_to( _time, _add )
#else
  void _Timestamp64_Add_to(
    Timestamp64_Control       *_time,
    const Timestamp64_Control *_add
  );
#endif

/**
 *  @brief Convert 64-bit timestamp to number of ticks.
 *
 *  This routine convert the @a time timestamp to the corresponding number
 *  of clock ticks.
 *
 *  @param[in] _time points to the time to be converted
 *
 *  @retval This method returns the number of ticks computed.
 */
uint32_t _Timestamp64_To_ticks(
  const Timestamp64_Control *_time
);

/**
 *  @brief Convert ticks to 64-bit timestamp.
 *
 *  This routine converts the @a _ticks value to the corresponding
 *  timestamp format @a _time.
 *
 *  @param[in] _time points to the timestamp format time result
 *  @param[out] _ticks points to the number of ticks to be filled in
 */
void _Timestamp64_From_ticks(
  uint32_t             _ticks,
  Timestamp64_Control *_time
);

static inline void _Timestamp64_implementation_Subtract(
  const Timestamp64_Control *_start,
  const Timestamp64_Control *_end,
  Timestamp64_Control       *_result
)
{
  *_result = *_end - *_start;
}

/**
 *  @brief Subtract two 64-bit timestamps.
 *
 *  This routine subtracts two timestamps.  @a result is set to
 *  @a end - @a start.
 *
 *  @param[in] _start points to the starting time
 *  @param[in] _end points to the ending time
 *  @param[out] _result points to the difference between
 *             starting and ending time.
 */
#if CPU_TIMESTAMP_USE_INT64_INLINE == TRUE
  #define _Timestamp64_Subtract( _start, _end, _result ) \
    _Timestamp64_implementation_Subtract( _start, _end, _result )
#else
  void _Timestamp64_Subtract(
    const Timestamp64_Control *_start,
    const Timestamp64_Control *_end,
    Timestamp64_Control       *_result
  );
#endif

static inline void _Timestamp64_implementation_Divide_by_integer(
  const Timestamp64_Control *_time,
  uint32_t             _iterations,
  Timestamp64_Control *_result
)
{
  *_result = *_time / _iterations;
}

/**
 *  @brief Divide 64-bit timestamp by an integer.
 *
 *  This routine divides a timestamp by an integer value.  The expected
 *  use is to assist in benchmark calculations where you typically
 *  divide a duration by a number of iterations.
 *
 *  @param[in] _time points to the total
 *  @param[in] _iterations is the number of iterations
 *  @param[out] _result points to the average time.
 */
#if CPU_TIMESTAMP_USE_INT64_INLINE == TRUE
  #define _Timestamp64_Divide_by_integer( _time, _iterations, _result ) \
    _Timestamp64_implementation_Divide_by_integer( _time, _iterations, _result )
#else
  void _Timestamp64_Divide_by_integer(
    const Timestamp64_Control *_time,
    uint32_t                   _iterations,
    Timestamp64_Control       *_result
  );
#endif

/**
 *  @brief Divide 64-bit timestamp by another 64-bit timestamp.
 *
 *  This routine divides a timestamp by another timestamp.  The
 *  intended use is for calculating percentages to three decimal points.
 *
 *  @param[in] _lhs points to the left hand number
 *  @param[in] _rhs points to the right hand number
 *  @param[out] _ival_percentage points to the integer portion of the average
 *  @param[out] _fval_percentage points to the thousandths of percentage
 */
void _Timestamp64_Divide(
  const Timestamp64_Control *_lhs,
  const Timestamp64_Control *_rhs,
  uint32_t                  *_ival_percentage,
  uint32_t                  *_fval_percentage
);

static inline uint32_t _Timestamp64_implementation_Get_seconds(
  const Timestamp64_Control *_time
)
{
  return (uint32_t) (*_time / 1000000000L);
}

/**
 *  @brief Get seconds portion of a 64-bit timestamp.
 *
 *  This method returns the seconds portion of the specified timestamp
 *
 *  @param[in] _time points to the timestamp
 *
 *  @retval The seconds portion of @a _time.
 */
#if CPU_TIMESTAMP_USE_INT64_INLINE == TRUE
  #define _Timestamp64_Get_seconds( _time ) \
    _Timestamp64_implementation_Get_seconds( _time )
#else
  uint32_t _Timestamp64_Get_seconds(
    const Timestamp64_Control *_time
  );
#endif

static inline uint32_t _Timestamp64_implementation_Get_nanoseconds(
  const Timestamp64_Control *_time
)
{
  return (uint32_t) (*_time % 1000000000L);
}

/**
 *  @brief Get nanoseconds portion of a 64-bit timestamp.
 *
 *  This method returns the nanoseconds portion of the specified timestamp
 *
 *  @param[in] _time points to the timestamp
 *
 *  @retval The nanoseconds portion of @a _time.
 */
#if CPU_TIMESTAMP_USE_INT64_INLINE == TRUE
  #define _Timestamp64_Get_nanoseconds( _time ) \
    _Timestamp64_implementation_Get_nanoseconds( _time )
#else
  uint32_t _Timestamp64_Get_nanoseconds(
    const Timestamp64_Control *_time
  );
#endif

static inline uint64_t _Timestamp64_implementation_Get_As_nanoseconds(
  const Timestamp64_Control *_time,
  const uint32_t nanoseconds
)
{
  return *_time + (uint64_t) nanoseconds;
}

/**
 *  @brief Get the 64-bit timestamp as nanoseconds.
 *
 *  This method returns the 64-bit timestamp as it is already in nanoseconds.
 *
 *  @param[in] _time points to the timestamp
 *
 *  @retval The nanoseconds portion of @a _time.
 */
#define _Timestamp64_Get_As_nanoseconds( _time, _nanoseconds ) \
  _Timestamp64_implementation_Get_As_nanoseconds( _time, _nanoseconds )

static inline void _Timestamp64_implementation_To_timespec(
  const Timestamp64_Control *_timestamp,
  struct timespec           *_timespec
)
{
  _timespec->tv_sec = (time_t) (*_timestamp / 1000000000L);
  _timespec->tv_nsec = (long) (*_timestamp % 1000000000L);
}

/**
 *  @brief Convert 64-bit timestamp to struct timespec.
 *
 *  This method returns the seconds portion of the specified timestamp
 *
 *  @param[in] _timestamp points to the timestamp
 *  @param[out] _timespec points to the timespec
 */
#if CPU_TIMESTAMP_USE_INT64_INLINE == TRUE
  #define _Timestamp64_To_timespec( _timestamp, _timespec  ) \
    _Timestamp64_implementation_To_timespec( _timestamp, _timespec )
#else
  void _Timestamp64_To_timespec(
    const Timestamp64_Control *_timestamp,
    struct timespec           *_timespec
  );
#endif

static inline void _Timestamp64_implementation_To_timeval(
  const Timestamp64_Control *_timestamp,
  struct timeval            *_timeval
)
{
  _timeval->tv_sec = (time_t) (*_timestamp / 1000000000U);
  _timeval->tv_usec = (suseconds_t) ((*_timestamp % 1000000000U) / 1000U);
}

/**
 *  @brief Convert 64-bit timestamp to struct timeval.
 *
 *  This method returns the seconds portion of the specified timestamp
 *
 *  @param[in] _timestamp points to the timestamp
 *  @param[out] _timeval points to the timeval
 */
#if CPU_TIMESTAMP_USE_INT64_INLINE == TRUE
  #define _Timestamp64_To_timeval( _timestamp, _timeval  ) \
    _Timestamp64_implementation_To_timeval( _timestamp, _timeval )
#else
  void _Timestamp64_To_timeval(
    const Timestamp64_Control *_timestamp,
    struct timeval            *_timeval
  );
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
