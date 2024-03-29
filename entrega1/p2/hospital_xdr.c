/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "hospital.h"

bool_t
xdr_init_data (XDR *xdrs, init_data *objp)
{
	register int32_t *buf;

	 if (!xdr_int (xdrs, &objp->num_quirofano))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->num_equipo))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_recursos (XDR *xdrs, recursos *objp)
{
	register int32_t *buf;

	 if (!xdr_int (xdrs, &objp->key_quirofanos))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->key_equipos))
		 return FALSE;
	return TRUE;
}
