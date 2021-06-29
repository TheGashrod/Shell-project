// https://www.gnu.org/software/libc/manual/html_node/Stopped-and-Terminated-Jobs.html



/* Check for processes that have status information available,
   blocking until all processes in the given job have reported.  */

void
wait_for_job (job *j)
{
  int status;
  waitpid (j->pgid, &status, WUNTRACED);
}
