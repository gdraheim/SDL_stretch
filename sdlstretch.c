/*
    SDL_stretch - Stretch Functions For The Simple DirectMedia Layer
    Copyright (C) 2003 Guido Draheim

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Guido Draheim, guidod@gmx.de
*/

#ifdef SAVE_RCSID
static char rcsid =
 "@(#) $Id: sdstretch.c $";
#endif

#include "SDL/SDL_error.h"
#include "SDL/SDL_video.h"
#include "SDL_stretch.h"
#include "SDL_stretchcode.h"

#define PRERUN 0


/* Perform a stretch blit between two surfaces of the same format.
   NOTE:  This function is not safe to call from multiple threads!
*/
int SDL_StretchSurfaceBlit(SDL_Surface *src, SDL_Rect *srcrect,
			   SDL_Surface *dst, SDL_Rect *dstrect)
{
	int i = 0;
	int src_row, dst_row;
	Uint8 *srcp = NULL;
	Uint8 *dstp;
	SDL_Rect rect;
#     if defined(USE_ASM_STRETCH) 
	char* code = 0;
#     endif
	const int bpp = dst->format->BytesPerPixel;
	auto int dest_w = srcrect->w * dst->w / src->w;
	auto int dest_x = srcrect->x * dst->w / src->w;

	if ( src->format->BitsPerPixel != dst->format->BitsPerPixel ) {
		SDL_SetError("Only works with same format surfaces");
		return(-1);
	}

	/* Verify the blit rectangles */
	if ( srcrect ) {
		if ( (srcrect->x < 0) || (srcrect->y < 0) ||
		     ((srcrect->x+srcrect->w) > src->w) ||
		     ((srcrect->y+srcrect->h) > src->h) ) {
			SDL_SetError("Invalid source blit rectangle");
			return(-1);
		}
	} else {
		rect.x = 0;
		rect.y = 0;
		rect.w = src->w;
		rect.h = src->h;
		srcrect = &rect;
	}

#      ifdef USE_ASM_STRETCH
	/* Write the opcodes for this stretch */
	/* if ( (bpp != 3) */
	code = SDL_SetStretchCode(srcrect->w, dest_w, bpp);
#      endif


	if (PRERUN)   /* let the compiler do the dead-code removal */
	{   /* Pre-Run the stretch blit (the invisible lines) */
	    src_row = 0; 
	    dst_row = 0;
	    while (1)
	    {
		if (src_row >= srcrect->y) break;
		dst_row++; i += src->h;                             
		if (i < dst->h) continue;                    
		do { i -= dst->h; src_row++; } while (i >= dst->h);
	    }   
	}else
	{   /* or compute the resulting dst_row and i-fraction directly: */
	    src_row = srcrect->y;
	    dst_row = ((src_row * dst->h)+(src->h-1)) / src->h;
	    i = (dst_row * src->h - src_row * dst->h ) % dst->h;
	    if (i < 0) i += dst->h;
	}

	if (dstrect) { /*returnvalue*/
	    dstrect->y = dst_row; 
	    dstrect->x = dest_x; 
	    dstrect->w = dest_w; 
	}


	while (src_row < srcrect->y + srcrect->h)
	{
	    srcp = (Uint8 *)src->pixels + (src_row*src->pitch)
		+ (srcrect->x*bpp);
	draw:
	    dstp = (Uint8 *)dst->pixels + (dst_row*dst->pitch)
		+ (dest_x*bpp);
#          ifdef USE_ASM_STRETCH
	    if (code)
	    {
#	       if defined __GNUC__
		__asm__ __volatile__ ("call SDL_TheRowStretchCode"
				      :: "S" (dstp), "D" (srcp) : "%eax");
#              elif defined WIN32
		__asm { 
		    push edi ;
		    push esi ;
		    mov edi, dstp ;
		    mov esi, srcp ;
		    call dword ptr code ;
		    pop esi ;
		    pop edi ;
		}
#              else
#              error Need inline assembly for this compiler
#              endif
	    }else
#          endif /*USE_ASM_STRETCH*/
	    { 
		switch (bpp) {
		case 1:
		    SDL_StretchRow1(srcp, srcrect->w, dstp, dest_w);
		    break;
		case 2:
		    SDL_StretchRow2((Uint16 *)srcp, srcrect->w,
				    (Uint16 *)dstp, dest_w);
		    break;
		case 3:
		    SDL_StretchRow3(srcp, srcrect->w, dstp, dest_w);
		    break;
		case 4:
		    SDL_StretchRow4((Uint32 *)srcp, srcrect->w,
				    (Uint32 *)dstp, dest_w);
		    break;
		}
	    }

	    dst_row++; i += src->h;                             
	    if (dst_row == dst->h) break; /*oops*/
	    if (i < dst->h) goto draw; /* draw the line again */
	    do { i -= dst->h; src_row++; } while (i >= dst->h);
	}

	if (dstrect) dstrect->h = dst_row - dstrect->y; /*returnvalue*/

	return(0);
}

/* ------------------------------------------------------------------------ */

#define OPTIMIZED 1

static int blit(int dst_row, int src_row, 
		SDL_Surface* dst, SDL_Surface* src,
		SDL_Rect* rect, SDL_Rect* clip, int dest_x, int i)
{
    const int bpp = src->format->BytesPerPixel;
    Uint8 *srcp = (Uint8 *)src->pixels + (src_row*src->pitch) + (rect->x*bpp);
    Uint8 *srcE = (Uint8 *)src->pixels + ((rect->y + rect->h) 
					  *src->pitch) + (rect->x*bpp);
    Uint8 *dstp = (Uint8 *)dst->pixels + (dst_row*dst->pitch) + (dest_x*bpp);
//    Uint8* dstE = (Uint8 *)dst->pixels + (dst->h*dst->pitch) + (dest_x*bpp);

	    while (srcp < srcE)
	    {
	    draws:
#	       if defined __GNUC__
		__asm__ __volatile__ ("call SDL_TheRowStretchCode"
				      :: "S" (dstp), "D" (srcp) : "%eax");
#              elif defined WIN32
		__asm { 
		    push edi ;
		    push esi ;
		    mov edi, dstp ;
		    mov esi, srcp ;
		    call dword ptr SDL_TheRowStretchCode ;
		    pop esi ;
		    pop edi ;
		}
#              else
#              error Need inline assembly for this compiler
#              endif
		dstp += dst->pitch; i += clip->h;                             
		// if (dstp == dstE) break; /*oops*/
		if (i < dst->h) goto draws; /* draw the line again */
		do { i -= dst->h; srcp += src->pitch; } while (i >= dst->h);
	    }
	    return dst_row;
}


#define DEBUGSIZES 0
#define	DEBUGSIZES_ (src->h != dst->h)

/* Perform a stretch blit between two surfaces of the same format.
   NOTE:  This function is not safe to call from multiple threads!
*/
int SDL_StretchSurfaceRect(SDL_Surface *src, SDL_Rect *srcrect,
			   SDL_Surface *dst, SDL_Rect *dstrect)
{
	int i = 0;
	int src_row, dst_row;
	int dest_x, dest_w;
	Uint8 *srcp = NULL;
	Uint8 *dstp;
	SDL_Rect rect;
	SDL_Rect clip;
#     if defined(USE_ASM_STRETCH) 
	char* code = 0;
#     endif
	const int bpp = src->format->BytesPerPixel;

	if ( src->format->BitsPerPixel != dst->format->BitsPerPixel ) {
		SDL_SetError("Only works with same format surfaces");
		return(-1);
	}

	/* Verify the blit rectangles */
	if ( srcrect ) {
	    rect = *srcrect;
#if 0
	    clip.x = clip.y = 0; clip.h = src->h; clip.w = src->w;
#else
	    clip = src->clip_rect;

	    if (rect.x > clip.x + clip.w ||
		rect.x + rect.w < clip.x ||
		rect.y > clip.y + clip.h ||
		rect.y + rect.h < clip.y) return 1;
	    if (rect.x < clip.x) { rect.w -= clip.x-rect.x; rect.x = clip.x; }
	    if (rect.w > clip.x + clip.w - rect.x)
		rect.w = clip.x + clip.w - rect.x;
	    if (rect.y < clip.y) { rect.h -= clip.y-rect.y; rect.y = clip.y; }
	    if (rect.h > clip.y + clip.h - rect.y)
		rect.h = clip.y + clip.h - rect.y;
#endif
	    if (! rect.h || !rect.w) return 1;
 	} else {
	    rect.x = 0;
	    rect.y = 0;
	    rect.w = src->w;
	    rect.h = src->h;
	    clip = rect;
	}

	if (DEBUGSIZES)
	    fprintf (stderr, "[%i/%i][%i+%i/%i+%i][%i+%i/%i+%i]\n", 
		     src->w,src->h,
		     clip.x,clip.w,clip.y,clip.h,
		     rect.x,rect.w,rect.y,rect.h);

	dest_w = rect.w * dst->w / clip.w;
	dest_x = (rect.x-clip.x) * dst->w / clip.w;

#      ifdef USE_ASM_STRETCH
	/* Write the opcodes for this stretch */
	/* if ( (bpp != 3) */
	code = SDL_SetStretchCode(rect.w, dest_w, bpp);
#      endif

	if (PRERUN)   /* let the compiler do the dead-code removal */
	{   /* Pre-Run the stretch blit (the invisible lines) */
	    src_row = clip.y; 
	    dst_row = 0;
	    while (1)
	    {
		if (src_row >= rect.y) break;
		dst_row++; i += clip.h;                             
		if (i < dst->h) continue;                    
		do { i -= dst->h; src_row++; } while (i >= dst->h);
	    }   
	}else
	{   /* or compute the resulting dst_row and i-fraction directly: */
	    src_row = rect.y - clip.y;
	    dst_row = ((src_row * dst->h)+(clip.h-1)) / clip.h;
	    i = (dst_row * clip.h - src_row * dst->h ) % dst->h;
	    if (i < 0) i += dst->h;
	}

	if (dstrect) { 
	    dstrect->y = dst_row; 
	    dstrect->x = dest_x; 
	    dstrect->w = dest_w; 
	}

	if (DEBUGSIZES)
	    fprintf (stderr, "[%i/%i][%i+%i/%i+%i][%i+%i/%i+%i]"
		     " -> [%i/%i][%i+%i/%i+?]\n", 
		     src->w,src->h,
		     clip.x,clip.w,clip.y,clip.h,
		     rect.x,rect.w,rect.y,rect.h,
		     dst->w,dst->h,
		     dest_x,dest_w,dst_row);

#ifdef USE_ASM_STRETCH
	if (code)
	{
#         ifdef OPTIMIZED
	    dst_row = blit(dst_row, src_row, dst, src, &rect, &clip, dest_x,i);
#         else
	    while (src_row < rect.y + rect.h)
	    {
		srcp = (Uint8 *)src->pixels + (src_row*src->pitch) 
		    + (rect.x*bpp);
	    draws:
		dstp = (Uint8 *)dst->pixels + (dst_row*dst->pitch) 
		    + (dest_x*bpp);

#	       if defined __GNUC__
		__asm__ __volatile__ ("call SDL_TheRowStretchCode"
				      :: "S" (dstp), "D" (srcp) : "%eax");
#              elif defined WIN32
		__asm { 
		    push edi ;
		    push esi ;
		    mov edi, dstp ;
		    mov esi, srcp ;
		    call dword ptr code ;
		    pop esi ;
		    pop edi ;
		}
#              else
#              error Need inline assembly for this compiler
#              endif
		dst_row++; i += clip.h;                             
		if (dst_row == dst->h) break; /*oops*/
		if (i < dst->h) goto draws; /* draw the line again */
		do { i -= dst->h; src_row++; } while (i >= dst->h);
	    }
#         endif
	}else
#          endif /*USE_ASM_STRETCH*/
	{
	    while (src_row < rect.y + rect.h)
	    {
		srcp = (Uint8 *)src->pixels + (src_row*src->pitch) 
		    + (rect.x*bpp);
	    draw:
		dstp = (Uint8 *)dst->pixels + (dst_row*dst->pitch) 
		    + (dest_x*bpp);

		if (DEBUGSIZES) fprintf(stderr, ".");
		switch (bpp) {
		case 1:
		    SDL_StretchRow1(srcp, rect.w, dstp, dest_w);
		    break;
		case 2:
		    SDL_StretchRow2((Uint16 *)srcp, rect.w,
				    (Uint16 *)dstp, dest_w);
		    break;
		case 3:
		    SDL_StretchRow3(srcp, rect.w, dstp, dest_w);
		    break;
		case 4:
		    SDL_StretchRow4((Uint32 *)srcp, rect.w,
				    (Uint32 *)dstp, dest_w);
		    break;
		}

		dst_row++; i += clip.h;                             
		if (dst_row == dst->h) break; /*oops*/
		if (i < dst->h) goto draw; /* draw the line again */
		do { i -= dst->h; src_row++; } while (i >= dst->h);
	    }
	}

	if (dstrect) dstrect->h = dst_row - dstrect->y; /*returnvalue*/

	if (DEBUGSIZES)
	    fprintf (stderr, "[%i/%i][%i+%i/%i+%i][%i+%i/%i+%i]"
		     " -> [%i/%i][%i+%i/?..%i]\n",
		     src->w,src->h,
		     clip.x,clip.w,clip.y,clip.h,
		     rect.x,rect.w,rect.y,rect.h,
		     dst->w,dst->h,
		     dest_x,dest_w,dst_row);

	return(0);
}
