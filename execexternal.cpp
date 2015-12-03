/*
 * Copyright © 2015 Filip Dorosz.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Filip Dorosz <filip.dorosz@rapidrage.org>
 * Version: ALPHA under development
 */

#include <ext/stdio_filebuf.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <fstream>

int main() {
    pid_t pid;
	int status = 0;
	int stdinpipe[2];
	int stdoutpipe[2];
	int stderrpipe[2];

	if (pipe(stdinpipe)) {
		std::cerr << "stdin pipe error" << std::endl;
		return 1;
	}

	if (pipe(stdoutpipe)) {
		std::cerr << "stdoutpipe pipe error" << std::endl;
		return 1;
	}

	if (pipe(stderrpipe)) {
		std::cerr << "stderr pipe error" << std::endl;
		return 1;
	}

	if ((pid=fork()) == -1) {
		std::cerr << "Fork error...." << std::endl;
		return 1;
	}

	if (pid) { //this is the parent scope

		int stdin_posix_handle = fileno (::fdopen( stdinpipe[1], "w" ));
		__gnu_cxx::stdio_filebuf<char> stdin_filebuf (stdin_posix_handle, std::ios::in);
		std::istream chcin(&stdin_filebuf);
		close (stdinpipe[0]);

		int stdout_posix_handle = fileno (::fdopen( stdoutpipe[0], "r" ));
		__gnu_cxx::stdio_filebuf<char> stdout_filebuf (stdout_posix_handle, std::ios::in);
		std::istream chcout(&stdout_filebuf);
		close (stdoutpipe[1]);

		int stderr_posix_handle = fileno (::fdopen( stderrpipe[0], "r" ));
		__gnu_cxx::stdio_filebuf<char> stderr_filebuf (stderr_posix_handle, std::ios::in);
		std::istream chcerr(&stderr_filebuf);
		close (stderrpipe[1]);

		while(!chcout.eof()){
		std::string str;
		chcout >> str;
		std::cout << str <<" ";
		}

		while(!chcerr.eof()){
		std::string str;
		chcerr >> str;
		std::cerr << str <<" ";
		}

		wait(&status);
		std::cout << "Child ended with code: " << status <<std::endl;
	}
	else { //this is child scope
		dup2(stdinpipe[0],0);
		close(stdinpipe[1]);

		dup2(stdoutpipe[1],1);
		close(stdoutpipe[0]);

		dup2(stderrpipe[1],2);
		close(stderrpipe[0]);

		if(execl("/bin/ls", "/bin/ls" "/etc/ssl/private", "/etc/ssl/private" , NULL) == -1){
			std::cerr << "execl error!\n";
		return 1;
		}
	}
	std::cout << "Execution ended" <<std::endl;
	return 0;
}
