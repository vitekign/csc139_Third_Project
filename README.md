<h1>The purpose of this project was to write code to simulate the following CPU scheduling algorithms: </h1> 
   <li> Shortest Job First (SJF) </li>
   <li> Round Robin (RR) </li>
   <li> Priority Scheduling without Preemption (PR_noPREMP): If a high-priority process gets the CPU, it will keep it until its CPU burst is complete. When a new process arrives, it will be added to the priority queue, but scheduling decisions are made only when the current process completes its CPU burst. </li> 
   <li> Priority Scheduling with Preemption (PR_withPREMP): If a high-priority process gets the CPU, it will keep it until either its CPU burst is complete or a higher priority process arrives. When a new process arrives, it is added to the priority queue and a scheduling decision is made immediately. If the new process has a higher priority than the process that currently has the CPU, the current process is preempted and the new process gets the CPU. If, on the other hand, the current process has a higher priority, it will keep the CPU. </li>
<br/>
<h4>input  [Process number | Arrival Time | CPU burst time | Priority</h4>
<img width="300" src="https://cloud.githubusercontent.com/assets/3220686/21170616/2d215b4a-c17b-11e6-9d9e-f6a3782daa9c.jpg"/>

<h4>output [process number | scheduled starting time]</h4>
<img width="300" src="https://cloud.githubusercontent.com/assets/3220686/21170622/364cba7a-c17b-11e6-94c2-fef0e0f70d7a.jpg"/>

<img width="300" src="https://cloud.githubusercontent.com/assets/3220686/21170624/3880e712-c17b-11e6-8c6a-4ac93e6421b1.jpg"/>

￼
