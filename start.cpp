#include "Server.hpp"



int main(int argc, __unused char **argv, char **envp)
{

	int j = 0;
	std::vector<Serv> servers_conf;
	char* _path = NULL;
	int z = 0;
	char *ret = NULL;
	while(envp[z])
	{
		if ((ret = strstr(envp[z], "PATH")))
		{
			_path = (envp[z]);
			break;
		}
		z++;
	}
	servers_conf = parser();
	z = 0;

	int numberOfServers = servers_conf.size();

	while(numberOfServers > z)
	{
		servers_conf[z].path = _path;
		z++;
	}
	if (argc == 1)
	{
		Server sr = Server();
		sr.setConfigs(servers_conf);
		while (numberOfServers > j)
		{
			try
			{
				ServerUnit unit = ServerUnit(servers_conf[j].port, servers_conf[j]);
				sr.addServerUnit(unit);
			}
			catch (std::exception &err){}
			j++;
		}

		sr.preprocess();

		do
		{
			if (sr.poll_process() == -1)
				exit(-1);
		} while (sr.get_end_server() == FALSE);
		if (sr.getCompressArr())
		{
			sr.setCompressArr(FALSE);
			sr.ft_compress_array();
		}
		sr.close_fds();
	}
	else
		std::cout << "Wrong number of arguments" << std::endl;
	return (1);
}