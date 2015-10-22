/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include <memory.h> /* for memset */
#include "rpcfunc.h"

/* Default timeout can be changed using clnt_control() */
static struct timeval TIMEOUT = { 25, 0 };

int_out *
remove_paper_1(int_in *argp, CLIENT *clnt)
{
	static int_out clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, REMOVE_PAPER,
		(xdrproc_t) xdr_int_in, (caddr_t) argp,
		(xdrproc_t) xdr_int_out, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

paper_data *
fetch_paper_1(int_in *argp, CLIENT *clnt)
{
	static paper_data clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, FETCH_PAPER,
		(xdrproc_t) xdr_int_in, (caddr_t) argp,
		(xdrproc_t) xdr_paper_data, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

paper_information *
info_paper_1(int_in *argp, CLIENT *clnt)
{
	static paper_information clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, INFO_PAPER,
		(xdrproc_t) xdr_int_in, (caddr_t) argp,
		(xdrproc_t) xdr_paper_information, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

paper_list_out *
list_paper_1(list_in *argp, CLIENT *clnt)
{
	static paper_list_out clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, LIST_PAPER,
		(xdrproc_t) xdr_list_in, (caddr_t) argp,
		(xdrproc_t) xdr_paper_list_out, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

int_out *
add_paper_1(paper_information *argp, CLIENT *clnt)
{
	static int_out clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, ADD_PAPER,
		(xdrproc_t) xdr_paper_information, (caddr_t) argp,
		(xdrproc_t) xdr_int_out, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}