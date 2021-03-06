/*
 * fogdeck.h
 *
 * Copyright (C) 1989, 1991, Craig E. Kolb
 * All rights reserved.
 *
 * This software may be freely copied, modified, and redistributed
 * provided that this copyright notice is preserved on all copies.
 *
 * You may not distribute this software, in whole or in part, as part of
 * any commercial product without the express consent of the authors.
 *
 * There is no warranty or other guarantee of fitness of this software
 * for any purpose.  It is provided solely "as is".
 *
 * $Id: fogdeck.h,v 1.1 2002-12-18 18:36:43 pepe Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 4.0  91/07/17  14:40:35  kolb
 * Initial version.
 * 
 */
#ifndef FOGDECK_H

#define AtmosFogdeckCreate(z,a,s,p,o,c,t) AtmosCreate( \
		(AtmosRef) FogdeckCreate(z,a,s,p,o,c,t), FogdeckApply)	
typedef struct Fogdeck {
	Float	alt,	/* Z value below which fogdeck deck exists. */
		offset,
		chaoscale;
	Vector	scale;
	Color	color,		/* Fogdeck color */
		trans;		/* Fogdeck transmissivity */
	int	octaves;
} Fogdeck;

extern Fogdeck * FogdeckCreate(
        Float           alt,
        Float           offset,
        Vector          *scale,
        Float           chaoscale,
        int             octaves,
        Color           *color,
        Color           *trans);
        
extern void FogdeckApply(Fogdeck *fogdeck, Ray *ray, Vector *pos, Float dist, Color *color);

#endif /* FOGDECK_H */
