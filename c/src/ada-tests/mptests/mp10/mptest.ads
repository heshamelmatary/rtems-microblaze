--
--  MPTEST / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package is the specification for Test 10 of the RTEMS
--  Multiprocessor Test Suite.
--
--  DEPENDENCIES: 
--
--  
--
--  COPYRIGHT (c) 1989-2011.
--  On-Line Applications Research Corporation (OAR).
--
--  The license and distribution terms for this file may in
--  the file LICENSE in this distribution or at
--  http://www.rtems.org/license/LICENSE.
--

with RTEMS;
with RTEMS.TASKS;

package MPTEST is

--
--  These arrays contain the IDs and NAMEs of all RTEMS tasks created
--  by this test.
--

   TASK_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 3 ) of RTEMS.ID;
   TASK_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 3 ) of RTEMS.NAME;

--
--  These arrays contain the IDs and NAMEs of all RTEMS message 
--  queues created by this test.
--

   QUEUE_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 2 ) of RTEMS.ID;
   QUEUE_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 2 ) of RTEMS.NAME;

--
--  These arrays contain the IDs and NAMEs of all RTEMS semaphore
--  created by this test.
--

   SEMAPHORE_ID   : array ( RTEMS.UNSIGNED32 range 1 .. 2 ) of RTEMS.ID;
   SEMAPHORE_NAME : array ( RTEMS.UNSIGNED32 range 1 .. 2 ) of RTEMS.NAME;

--
--  This variable contains the ID of the remote task with which this
--  test interacts.
--

   REMOTE_TID  : RTEMS.ID;

--
--  This variable contains the node on which the remote task with which 
--  this test interacts resides.
--

   REMOTE_NODE : RTEMS.UNSIGNED32;

--
--  The number of events to process per dot printed out.
--

   PER_DOT : constant RTEMS.UNSIGNED32 := 100;

--
--  INIT
--
--  DESCRIPTION:
--
--  This RTEMS task initializes the application.
--

   procedure INIT (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, INIT);

--
--  TEST_TASK_1
--
--  DESCRIPTION:
--
--  This is the body of one of the RTEMS tasks which constitute this test.
--

   procedure TEST_TASK_1 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, TEST_TASK_1);

--
--  TEST_TASK_2  
--
--  DESCRIPTION: 
--
--  This is the body of one of the RTEMS tasks which constitute this test.
--
 
   procedure TEST_TASK_2 ( 
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, TEST_TASK_2);

--
--  TEST_TASK_3  
--
--  DESCRIPTION: 
--
--  This is the body of one of the RTEMS tasks which constitute this test.
--
 
   procedure TEST_TASK_3 ( 
      RESTART : in     RTEMS.TASKS.ARGUMENT
   );
   pragma Convention (C, TEST_TASK_3);

end MPTEST;
