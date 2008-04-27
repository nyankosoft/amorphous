//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file distance.cpp 
//                     
//==============================================================
#include "distance.hpp"
#include "3DMath/Triangle.h"
#include "Box.h"
#include <limits>
using namespace JigLib;

//==============================================================
// SegmentSegmentDistanceSq
//==============================================================
Scalar JigLib::SegmentSegmentDistanceSq(Scalar *t0, Scalar *t1, 
                                         const tSegment & seg0, const tSegment & seg1)
{
  Vector3 kDiff = seg0.mOrigin - seg1.mOrigin;

//  Scalar fA00 = seg0.mDelta.GetLengthSq();
//  Scalar fA01 = -Vec3Dot(seg0.mDelta, seg1.mDelta);
//  Scalar fA11 = seg1.mDelta.GetLengthSq();
//  Scalar fB0 = Vec3Dot(kDiff, seg0.mDelta);
//  Scalar fC = kDiff.GetLengthSq();
//  Scalar fDet = Abs(fA00*fA11-fA01*fA01);

  Scalar fA00 = Vec3LengthSq( seg0.mDelta );
  Scalar fA01 = -Vec3Dot(seg0.mDelta, seg1.mDelta);
  Scalar fA11 = Vec3LengthSq( seg1.mDelta );
  Scalar fB0 = Vec3Dot(kDiff, seg0.mDelta);
  Scalar fC = Vec3LengthSq( kDiff );
  Scalar fDet = fabsf(fA00*fA11-fA01*fA01);

  Scalar fB1, fS, fT, fSqrDist, fTmp;

  if ( fDet >= SCALAR_TINY )
  {
    // line segments are not parallel
//    fB1 = -Vec3Dot(kDiff, seg1.mDelta);
    fB1 = -Vec3Dot(&kDiff, &seg1.mDelta);
    fS = fA01*fB1-fA11*fB0;
    fT = fA01*fB0-fA00*fB1;

    if ( fS >= (Scalar)0.0 )
    {
      if ( fS <= fDet )
      {
        if ( fT >= (Scalar)0.0 )
        {
          if ( fT <= fDet )  // region 0 (interior)
          {
            // minimum at two interior points of 3D lines
            Scalar fInvDet = ((Scalar)1.0)/fDet;
            fS *= fInvDet;
            fT *= fInvDet;
            fSqrDist = fS*(fA00*fS+fA01*fT+((Scalar)2.0)*fB0) +
              fT*(fA01*fS+fA11*fT+((Scalar)2.0)*fB1)+fC;
          }
          else  // region 3 (side)
          {
            fT = (Scalar)1.0;
            fTmp = fA01+fB0;
            if ( fTmp >= (Scalar)0.0 )
            {
              fS = (Scalar)0.0;
              fSqrDist = fA11+((Scalar)2.0)*fB1+fC;
            }
            else if ( -fTmp >= fA00 )
            {
              fS = (Scalar)1.0;
              fSqrDist = fA00+fA11+fC+((Scalar)2.0)*(fB1+fTmp);
            }
            else
            {
              fS = -fTmp/fA00;
              fSqrDist = fTmp*fS+fA11+((Scalar)2.0)*fB1+fC;
            }
          }
        }
        else  // region 7 (side)
        {
          fT = (Scalar)0.0;
          if ( fB0 >= (Scalar)0.0 )
          {
            fS = (Scalar)0.0;
            fSqrDist = fC;
          }
          else if ( -fB0 >= fA00 )
          {
            fS = (Scalar)1.0;
            fSqrDist = fA00+((Scalar)2.0)*fB0+fC;
          }
          else
          {
            fS = -fB0/fA00;
            fSqrDist = fB0*fS+fC;
          }
        }
      }
      else
      {
        if ( fT >= (Scalar)0.0 )
        {
          if ( fT <= fDet )  // region 1 (side)
          {
            fS = (Scalar)1.0;
            fTmp = fA01+fB1;
            if ( fTmp >= (Scalar)0.0 )
            {
              fT = (Scalar)0.0;
              fSqrDist = fA00+((Scalar)2.0)*fB0+fC;
            }
            else if ( -fTmp >= fA11 )
            {
              fT = (Scalar)1.0;
              fSqrDist = fA00+fA11+fC+((Scalar)2.0)*(fB0+fTmp);
            }
            else
            {
              fT = -fTmp/fA11;
              fSqrDist = fTmp*fT+fA00+((Scalar)2.0)*fB0+fC;
            }
          }
          else  // region 2 (corner)
          {
            fTmp = fA01+fB0;
            if ( -fTmp <= fA00 )
            {
              fT = (Scalar)1.0;
              if ( fTmp >= (Scalar)0.0 )
              {
                fS = (Scalar)0.0;
                fSqrDist = fA11+((Scalar)2.0)*fB1+fC;
              }
              else
              {
                fS = -fTmp/fA00;
                fSqrDist = fTmp*fS+fA11+((Scalar)2.0)*fB1+fC;
              }
            }
            else
            {
              fS = (Scalar)1.0;
              fTmp = fA01+fB1;
              if ( fTmp >= (Scalar)0.0 )
              {
                fT = (Scalar)0.0;
                fSqrDist = fA00+((Scalar)2.0)*fB0+fC;
              }
              else if ( -fTmp >= fA11 )
              {
                fT = (Scalar)1.0;
                fSqrDist = fA00+fA11+fC+
                  ((Scalar)2.0)*(fB0+fTmp);
              }
              else
              {
                fT = -fTmp/fA11;
                fSqrDist = fTmp*fT+fA00+((Scalar)2.0)*fB0+fC;
              }
            }
          }
        }
        else  // region 8 (corner)
        {
          if ( -fB0 < fA00 )
          {
            fT = (Scalar)0.0;
            if ( fB0 >= (Scalar)0.0 )
            {
              fS = (Scalar)0.0;
              fSqrDist = fC;
            }
            else
            {
              fS = -fB0/fA00;
              fSqrDist = fB0*fS+fC;
            }
          }
          else
          {
            fS = (Scalar)1.0;
            fTmp = fA01+fB1;
            if ( fTmp >= (Scalar)0.0 )
            {
              fT = (Scalar)0.0;
              fSqrDist = fA00+((Scalar)2.0)*fB0+fC;
            }
            else if ( -fTmp >= fA11 )
            {
              fT = (Scalar)1.0;
              fSqrDist = fA00+fA11+fC+((Scalar)2.0)*(fB0+fTmp);
            }
            else
            {
              fT = -fTmp/fA11;
              fSqrDist = fTmp*fT+fA00+((Scalar)2.0)*fB0+fC;
            }
          }
        }
      }
    }
    else 
    {
      if ( fT >= (Scalar)0.0 )
      {
        if ( fT <= fDet )  // region 5 (side)
        {
          fS = (Scalar)0.0;
          if ( fB1 >= (Scalar)0.0 )
          {
            fT = (Scalar)0.0;
            fSqrDist = fC;
          }
          else if ( -fB1 >= fA11 )
          {
            fT = (Scalar)1.0;
            fSqrDist = fA11+((Scalar)2.0)*fB1+fC;
          }
          else
          {
            fT = -fB1/fA11;
            fSqrDist = fB1*fT+fC;
          }
        }
        else  // region 4 (corner)
        {
          fTmp = fA01+fB0;
          if ( fTmp < (Scalar)0.0 )
          {
            fT = (Scalar)1.0;
            if ( -fTmp >= fA00 )
            {
              fS = (Scalar)1.0;
              fSqrDist = fA00+fA11+fC+((Scalar)2.0)*(fB1+fTmp);
            }
            else
            {
              fS = -fTmp/fA00;
              fSqrDist = fTmp*fS+fA11+((Scalar)2.0)*fB1+fC;
            }
          }
          else
          {
            fS = (Scalar)0.0;
            if ( fB1 >= (Scalar)0.0 )
            {
              fT = (Scalar)0.0;
              fSqrDist = fC;
            }
            else if ( -fB1 >= fA11 )
            {
              fT = (Scalar)1.0;
              fSqrDist = fA11+((Scalar)2.0)*fB1+fC;
            }
            else
            {
              fT = -fB1/fA11;
              fSqrDist = fB1*fT+fC;
            }
          }
        }
      }
      else   // region 6 (corner)
      {
        if ( fB0 < (Scalar)0.0 )
        {
          fT = (Scalar)0.0;
          if ( -fB0 >= fA00 )
          {
            fS = (Scalar)1.0;
            fSqrDist = fA00+((Scalar)2.0)*fB0+fC;
          }
          else
          {
            fS = -fB0/fA00;
            fSqrDist = fB0*fS+fC;
          }
        }
        else
        {
          fS = (Scalar)0.0;
          if ( fB1 >= (Scalar)0.0 )
          {
            fT = (Scalar)0.0;
            fSqrDist = fC;
          }
          else if ( -fB1 >= fA11 )
          {
            fT = (Scalar)1.0;
            fSqrDist = fA11+((Scalar)2.0)*fB1+fC;
          }
          else
          {
            fT = -fB1/fA11;
            fSqrDist = fB1*fT+fC;
          }
        }
      }
    }
  }
  else
  {
    // line segments are parallel
    if ( fA01 > (Scalar)0.0 )
    {
      // direction vectors form an obtuse angle
      if ( fB0 >= (Scalar)0.0 )
      {
        fS = (Scalar)0.0;
        fT = (Scalar)0.0;
        fSqrDist = fC;
      }
      else if ( -fB0 <= fA00 )
      {
        fS = -fB0/fA00;
        fT = (Scalar)0.0;
        fSqrDist = fB0*fS+fC;
      }
      else
      {
//        fB1 = -Vec3Dot(kDiff, seg1.mDelta);
        fB1 = -Vec3Dot(&kDiff, &seg1.mDelta);
        fS = (Scalar)1.0;
        fTmp = fA00+fB0;
        if ( -fTmp >= fA01 )
        {
          fT = (Scalar)1.0;
          fSqrDist = fA00+fA11+fC+((Scalar)2.0)*(fA01+fB0+fB1);
        }
        else
        {
          fT = -fTmp/fA01;
          fSqrDist = fA00+((Scalar)2.0)*fB0+fC+fT*(fA11*fT+
                                                    ((Scalar)2.0)*(fA01+fB1));
        }
      }
    }
    else
    {
      // direction vectors form an acute angle
      if ( -fB0 >= fA00 )
      {
        fS = (Scalar)1.0;
        fT = (Scalar)0.0;
        fSqrDist = fA00+((Scalar)2.0)*fB0+fC;
      }
      else if ( fB0 <= (Scalar)0.0 )
      {
        fS = -fB0/fA00;
        fT = (Scalar)0.0;
        fSqrDist = fB0*fS+fC;
      }
      else
      {
//        fB1 = -Vec3Dot(kDiff, seg1.mDelta);
        fB1 = -Vec3Dot(&kDiff, &seg1.mDelta);
        fS = (Scalar)0.0;
        if ( fB0 >= -fA01 )
        {
          fT = (Scalar)1.0;
          fSqrDist = fA11+((Scalar)2.0)*fB1+fC;
        }
        else
        {
          fT = -fB0/fA01;
          fSqrDist = fC+fT*(((Scalar)2.0)*fB1+fA11*fT);
        }
      }
    }
  }

  if ( t0 )
    *t0 = fS;

  if ( t1 )
    *t1 = fT;

//  return Abs(fSqrDist);
  return fabsf(fSqrDist);
}

/*
//==============================================================
// PointRectangleDistance
//==============================================================
Scalar JigLib::PointRectangleDistanceSq(Scalar * pfSParam, Scalar * pfTParam, const Vector3 & rkPoint, const tRectangle & rkRct)
{
  Vector3 kDiff = rkRct.mOrigin - rkPoint;
//  Scalar fA00 = rkRct.mEdge0.GetLengthSq();
//  Scalar fA11 = rkRct.mEdge1.GetLengthSq();
  Scalar fA00 = Vec3LengthSq( rkRct.mEdge0 );
  Scalar fA11 = Vec3LengthSq( rkRct.mEdge1 );
  Scalar fB0 = Vec3Dot(kDiff, rkRct.mEdge0);
  Scalar fB1 = Vec3Dot(kDiff, rkRct.mEdge1);
  Scalar fS = -fB0, fT = -fB1;

//  Scalar fSqrDist = kDiff.GetLengthSq();
  Scalar fSqrDist = Vec3LengthSq( kDiff );

  if ( fS < (Scalar)0.0 )
  {
    fS = (Scalar)0.0;
  }
  else if ( fS <= fA00 )
  {
    fS /= fA00;
    fSqrDist += fB0*fS;
  }
  else
  {
    fS = (Scalar)1.0;
    fSqrDist += fA00 + ((Scalar)2.0)*fB0;
  }

  if ( fT < (Scalar)0.0 )
  {
    fT = (Scalar)0.0;
  }
  else if ( fT <= fA11 )
  {
    fT /= fA11;
    fSqrDist += fB1*fT;
  }
  else
  {
    fT = (Scalar)1.0;
    fSqrDist += fA11 + ((Scalar)2.0)*fB1;
  }

  if ( pfSParam )
    *pfSParam = fS;

  if ( pfTParam )
    *pfTParam = fT;

  return Abs(fSqrDist);

}


//==============================================================
// SegmentRectDistanceSq
//==============================================================
Scalar JigLib::SegmentRectDistanceSq(Scalar* pfSegP, Scalar* pfRctP0, Scalar* pfRctP1,
                                      const tSegment & seg, const tRectangle & rect)
{
  Vector3 kDiff = rect.mOrigin - seg.mOrigin;
//  Scalar fA00 = seg.mDelta.GetLengthSq();
  Scalar fA00 = Vec3LengthSq( seg.mDelta );
  Scalar fA01 = -Vec3Dot(seg.mDelta, rect.mEdge0);
  Scalar fA02 = -Vec3Dot(seg.mDelta, rect.mEdge1);
//  Scalar fA11 = rect.mEdge0.GetLengthSq();
//  Scalar fA22 = rect.mEdge1.GetLengthSq();
  Scalar fA11 = Vec3LengthSq( rect.mEdge0 );
  Scalar fA22 = Vec3LengthSq( rect.mEdge1 );
  Scalar fB0  = -Vec3Dot(kDiff, seg.mDelta);
  Scalar fB1  = Vec3Dot(kDiff, rect.mEdge0);
  Scalar fB2  = Vec3Dot(kDiff, rect.mEdge1);
  Scalar fCof00 = fA11*fA22;
  Scalar fCof01 = -fA01*fA22;
  Scalar fCof02 = -fA02*fA11;
  Scalar fDet = fA00*fCof00+fA01*fCof01+fA02*fCof02;

  tSegment kSegPgm;
  Vector3 kPt;
  Scalar fSqrDist, fSqrDist0, fR, fS, fT, fR0, fS0, fT0;

  if ( Abs(fDet) >= SCALAR_TINY )
  {
    Scalar fCof11 = fA00*fA22-fA02*fA02;
    Scalar fCof12 = fA02*fA01;
    Scalar fCof22 = fA00*fA11-fA01*fA01;
    Scalar fInvDet = ((Scalar)1.0)/fDet;
    Scalar fRhs0 = -fB0*fInvDet;
    Scalar fRhs1 = -fB1*fInvDet;
    Scalar fRhs2 = -fB2*fInvDet;

    fR = fCof00*fRhs0+fCof01*fRhs1+fCof02*fRhs2;
    fS = fCof01*fRhs0+fCof11*fRhs1+fCof12*fRhs2;
    fT = fCof02*fRhs0+fCof12*fRhs1+fCof22*fRhs2;

    if ( fR < (Scalar)0.0 )
    {
      if ( fS < (Scalar)0.0 )
      {
        if ( fT < (Scalar)0.0 )  // region 6m
        {
          // min on face s=0 or t=0 or r=0
          kSegPgm.mOrigin = rect.mOrigin;
          kSegPgm.mDelta = rect.mEdge1;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fT, seg, kSegPgm);
          //          fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fT);
          fS = (Scalar)0.0;
          kSegPgm.mOrigin = rect.mOrigin;
          kSegPgm.mDelta = rect.mEdge0;
          fSqrDist0 = SegmentSegmentDistanceSq(&fR0, &fS0, seg, kSegPgm);
          //          fSqrDist0 = SqrDistance(seg,kSegPgm,&fR0,&fS0);
          fT0 = (Scalar)0.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
          fSqrDist0 = PointRectangleDistanceSq(&fS0, &fT0, seg.mOrigin, rect);
          //          fSqrDist0 = SqrDistance(seg.mOrigin,rkRct,&fS0,&fT0);
          fR0 = (Scalar)0.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
        else if ( fT <= (Scalar)1.0 )  // region 5m
        {
          // min on face s=0 or r=0
          kSegPgm.mOrigin = rect.mOrigin;
          kSegPgm.mDelta = rect.mEdge1;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fT, seg, kSegPgm);
          //          fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fT);
          fS = (Scalar)0.0;
          fSqrDist0 = PointRectangleDistanceSq(&fS0, &fT0, seg.mOrigin, rect);
          //          fSqrDist0 = SqrDistance(seg.mOrigin,rkRct,&fS0,&fT0);
          fR0 = (Scalar)0.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
        else  // region 4m
        {
          // min on face s=0 or t=1 or r=0
          kSegPgm.mOrigin = rect.mOrigin;
          kSegPgm.mDelta = rect.mEdge1;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fT, seg, kSegPgm);
          //          fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fT);
          fS = (Scalar)0.0;
          kSegPgm.mOrigin = rect.mOrigin+rect.mEdge1;
          kSegPgm.mDelta = rect.mEdge0;
          fSqrDist0 = SegmentSegmentDistanceSq(&fR0, &fS0, seg, kSegPgm);
          //          fSqrDist0 = SqrDistance(seg,kSegPgm,&fR0,&fS0);
          fT0 = (Scalar)1.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
          fSqrDist0 = PointRectangleDistanceSq(&fS0, &fT0, seg.mOrigin, rect);
          //          fSqrDist0 = SqrDistance(seg.mOrigin,rkRct,&fS0,&fT0);
          fR0 = (Scalar)0.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
      }
      else if ( fS <= (Scalar)1.0 )
      {
        if ( fT < (Scalar)0.0 )  // region 7m
        {
          // min on face t=0 or r=0
          kSegPgm.mOrigin = rect.mOrigin;
          kSegPgm.mDelta = rect.mEdge0;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fT, seg, kSegPgm);
          //          fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fS);
          fT = (Scalar)0.0;
          fSqrDist0 = PointRectangleDistanceSq(&fS0, &fT0, seg.mOrigin, rect);
          //          fSqrDist0 = SqrDistance(seg.mOrigin,rkRct,&fS0,&fT0);
          fR0 = (Scalar)0.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
        else if ( fT <= (Scalar)1.0 )  // region 0m
        {
          // min on face r=0
          fSqrDist = PointRectangleDistanceSq(&fS, &fT, seg.mOrigin, rect);
          //          fSqrDist = SqrDistance(seg.mOrigin,rkRct,&fS,&fT);
          fR = (Scalar)0.0;
        }
        else  // region 3m
        {
          // min on face t=1 or r=0
          kSegPgm.mOrigin = rect.mOrigin+rect.mEdge1;
          kSegPgm.mDelta = rect.mEdge0;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fT, seg, kSegPgm);
          //          fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fS);
          fT = (Scalar)1.0;
          fSqrDist0 = PointRectangleDistanceSq(&fS0, &fT0, seg.mOrigin, rect);
          //          fSqrDist0 = SqrDistance(seg.mOrigin,rkRct,&fS0,&fT0);
          fR0 = (Scalar)0.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
      }
      else
      {
        if ( fT < (Scalar)0.0 )  // region 8m
        {
          // min on face s=1 or t=0 or r=0
          kSegPgm.mOrigin = rect.mOrigin+rect.mEdge0;
          kSegPgm.mDelta = rect.mEdge1;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fT, seg, kSegPgm);
          //          fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fT);
          fS = (Scalar)1.0;
          kSegPgm.mOrigin = rect.mOrigin;
          kSegPgm.mDelta = rect.mEdge0;
          fSqrDist0 = SegmentSegmentDistanceSq(&fR0, &fS0, seg, kSegPgm);
          //          fSqrDist0 = SqrDistance(seg,kSegPgm,&fR0,&fS0);
          fT0 = (Scalar)0.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
          fSqrDist0 = PointRectangleDistanceSq(&fS0, &fT0, seg.mOrigin, rect);
          //          fSqrDist0 = SqrDistance(seg.mOrigin,rkRct,&fS0,&fT0);
          fR0 = (Scalar)0.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
        else if ( fT <= (Scalar)1.0 )  // region 1m
        {
          // min on face s=1 or r=0
          kSegPgm.mOrigin = rect.mOrigin+rect.mEdge0;
          kSegPgm.mDelta = rect.mEdge1;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fT, seg, kSegPgm);
          //          fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fT);
          fS = (Scalar)1.0;
          fSqrDist0 = PointRectangleDistanceSq(&fS0, &fT0, seg.mOrigin, rect);
          //          fSqrDist0 = SqrDistance(seg.mOrigin,rkRct,&fS0,&fT0);
          fR0 = (Scalar)0.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
        else  // region 2m
        {
          // min on face s=1 or t=1 or r=0
          kSegPgm.mOrigin = rect.mOrigin+rect.mEdge0;
          kSegPgm.mDelta = rect.mEdge1;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fT, seg, kSegPgm);
          //          fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fT);
          fS = (Scalar)1.0;
          kSegPgm.mOrigin = rect.mOrigin+rect.mEdge1;
          kSegPgm.mDelta = rect.mEdge0;
          fSqrDist0 = SegmentSegmentDistanceSq(&fR0, &fS0, seg, kSegPgm);
          //          fSqrDist0 = SqrDistance(seg,kSegPgm,&fR0,&fS0);
          fT0 = (Scalar)1.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
          fSqrDist0 = PointRectangleDistanceSq(&fS0, &fT0, seg.mOrigin, rect);
          //          fSqrDist0 = SqrDistance(seg.mOrigin,rkRct,&fS0,&fT0);
          fR0 = (Scalar)0.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
      }
    }
    else if ( fR <= (Scalar)1.0 )
    {
      if ( fS < (Scalar)0.0 )
      {
        if ( fT < (Scalar)0.0 )  // region 6
        {
          // min on face s=0 or t=0
          kSegPgm.mOrigin = rect.mOrigin;
          kSegPgm.mDelta = rect.mEdge1;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fT, seg, kSegPgm);
          //              fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fT);
          fS = (Scalar)0.0;
          kSegPgm.mOrigin = rect.mOrigin;
          kSegPgm.mDelta = rect.mEdge0;
          fSqrDist0 = SegmentSegmentDistanceSq(&fR0, &fS0, seg, kSegPgm);
          //              fSqrDist0 = SqrDistance(seg,kSegPgm,&fR0,&fS0);
          fT0 = (Scalar)0.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
        else if ( fT <= (Scalar)1.0 )  // region 5
        {
          // min on face s=0
          kSegPgm.mOrigin = rect.mOrigin;
          kSegPgm.mDelta = rect.mEdge1;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fT, seg, kSegPgm);
          //              fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fT);
          fS = (Scalar)0.0;
        }
        else // region 4
        {
          // min on face s=0 or t=1
          kSegPgm.mOrigin = rect.mOrigin;
          kSegPgm.mDelta = rect.mEdge1;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fT, seg, kSegPgm);
          //              fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fT);
          fS = (Scalar)0.0;
          kSegPgm.mOrigin = rect.mOrigin+rect.mEdge1;
          kSegPgm.mDelta = rect.mEdge0;
          fSqrDist0 = SegmentSegmentDistanceSq(&fR0, &fS0, seg, kSegPgm);
          //              fSqrDist0 = SqrDistance(seg,kSegPgm,&fR0,&fS0);
          fT0 = (Scalar)1.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
      }
      else if ( fS <= (Scalar)1.0 )
      {
        if ( fT < (Scalar)0.0 )  // region 7
        {
          // min on face t=0
          kSegPgm.mOrigin = rect.mOrigin;
          kSegPgm.mDelta = rect.mEdge0;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fS, seg, kSegPgm);
          //              fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fS);
          fT = (Scalar)0.0;
        }
        else if ( fT <= (Scalar)1.0 )  // region 0
        {
          // global minimum is interior
          fSqrDist = fR*(fA00*fR+fA01*fS+fA02*fT+((Scalar)2.0)*fB0)
            +fS*(fA01*fR+fA11*fS+((Scalar)2.0)*fB1)
            +fT*(fA02*fR+fA22*fT+((Scalar)2.0)*fB2)
//            +kDiff.GetLengthSq();
            +Vec3LengthSq( kDiff );
        }
        else  // region 3
        {
          // min on face t=1
          kSegPgm.mOrigin = rect.mOrigin+rect.mEdge1;
          kSegPgm.mDelta = rect.mEdge0;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fS, seg, kSegPgm);
          //              fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fS);
          fT = (Scalar)1.0;
        }
      }
      else
      {
        if ( fT < 0.0 )  // region 8
        {
          // min on face s=1 or t=0
          kSegPgm.mOrigin = rect.mOrigin+rect.mEdge0;
          kSegPgm.mDelta = rect.mEdge1;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fT, seg, kSegPgm);
          //              fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fT);
          fS = (Scalar)1.0;
          kSegPgm.mOrigin = rect.mOrigin;
          kSegPgm.mDelta = rect.mEdge0;
          fSqrDist0 = SegmentSegmentDistanceSq(&fR0, &fS0, seg, kSegPgm);
          //              fSqrDist0 = SqrDistance(seg,kSegPgm,&fR0,&fS0);
          fT0 = (Scalar)0.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
        else if ( fT <= (Scalar)1.0 )  // region 1
        {
          // min on face s=1
          kSegPgm.mOrigin = rect.mOrigin+rect.mEdge0;
          kSegPgm.mDelta = rect.mEdge1;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fT, seg, kSegPgm);
          //              fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fT);
          fS = (Scalar)1.0;
        }
        else  // region 2
        {
          // min on face s=1 or t=1
          kSegPgm.mOrigin = rect.mOrigin+rect.mEdge0;
          kSegPgm.mDelta = rect.mEdge1;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fT, seg, kSegPgm);
          //              fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fT);
          fS = (Scalar)1.0;
          kSegPgm.mOrigin = rect.mOrigin+rect.mEdge1;
          kSegPgm.mDelta = rect.mEdge0;
          fSqrDist0 = SegmentSegmentDistanceSq(&fR0, &fS0, seg, kSegPgm);
          //              fSqrDist0 = SqrDistance(seg,kSegPgm,&fR0,&fS0);
          fT0 = (Scalar)1.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
      }
    }
    else
    {
      if ( fS < (Scalar)0.0 )
      {
        if ( fT < (Scalar)0.0 )  // region 6p
        {
          // min on face s=0 or t=0 or r=1
          kSegPgm.mOrigin = rect.mOrigin;
          kSegPgm.mDelta = rect.mEdge1;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fT, seg, kSegPgm);
          //              fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fT);
          fS = (Scalar)0.0;
          kSegPgm.mOrigin = rect.mOrigin;
          kSegPgm.mDelta = rect.mEdge0;
          fSqrDist0 = SegmentSegmentDistanceSq(&fR0, &fS0, seg, kSegPgm);
          //              fSqrDist0 = SqrDistance(seg,kSegPgm,&fR0,&fS0);
          fT0 = (Scalar)0.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
          kPt = seg.mOrigin + seg.mDelta;
          fSqrDist0 = PointRectangleDistanceSq(&fS0, &fT0, kPt, rect);
          //              fSqrDist0 = SqrDistance(kPt,rkRct,&fS0,&fT0);
          fR0 = (Scalar)1.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
        else if ( fT <= (Scalar)1.0 )  // region 5p
        {
          // min on face s=0 or r=1
          kSegPgm.mOrigin = rect.mOrigin;
          kSegPgm.mDelta = rect.mEdge1;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fT, seg, kSegPgm);
          //              fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fT);
          fS = (Scalar)0.0;
          fSqrDist0 = PointRectangleDistanceSq(&fS0, &fT0, seg.mOrigin, rect);
          //              fSqrDist0 = SqrDistance(seg.mOrigin,rkRct,&fS0,&fT0);
          kPt = seg.mOrigin+seg.mDelta;
          fSqrDist0 = PointRectangleDistanceSq(&fS0, &fT0, kPt, rect);
          //              fSqrDist0 = SqrDistance(kPt,rkRct,&fS0,&fT0);
          fR0 = (Scalar)1.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
        else  // region 4p
        {
          // min on face s=0 or t=1 or r=1
          kSegPgm.mOrigin = rect.mOrigin;
          kSegPgm.mDelta = rect.mEdge1;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fT, seg, kSegPgm);
          //              fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fT);
          fS = (Scalar)0.0;
          kSegPgm.mOrigin = rect.mOrigin+rect.mEdge1;
          kSegPgm.mDelta = rect.mEdge0;
          fSqrDist0 = SegmentSegmentDistanceSq(&fR0, &fS0, seg, kSegPgm);
          //              fSqrDist0 = SqrDistance(seg,kSegPgm,&fR0,&fS0);
          fT0 = (Scalar)1.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
          kPt = seg.mOrigin+seg.mDelta;
          fSqrDist = PointRectangleDistanceSq(&fS0, &fT0, kPt, rect);
          //              fSqrDist0 = SqrDistance(kPt,rkRct,&fS0,&fT0);
          fR0 = (Scalar)1.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
      }
      else if ( fS <= (Scalar)1.0 )
      {
        if ( fT < (Scalar)0.0 )  // region 7p
        {
          // min on face t=0 or r=1
          kSegPgm.mOrigin = rect.mOrigin;
          kSegPgm.mDelta = rect.mEdge0;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fS, seg, kSegPgm);
          //              fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fS);
          fT = (Scalar)0.0;
          kPt = seg.mOrigin+seg.mDelta;
          fSqrDist0 = PointRectangleDistanceSq(&fS0, &fT0, kPt, rect);
          //              fSqrDist0 = SqrDistance(kPt,rkRct,&fS0,&fT0);
          fR0 = (Scalar)1.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
        else if ( fT <= (Scalar)1.0 )  // region 0p
        {
          // min on face r=1
          kPt = seg.mOrigin+seg.mDelta;
          fSqrDist = PointRectangleDistanceSq(&fS, &fT, kPt, rect);
          //              fSqrDist = SqrDistance(kPt,rkRct,&fS,&fT);
          fR = (Scalar)1.0;
        }
        else  // region 3p
        {
          // min on face t=1 or r=1
          kSegPgm.mOrigin = rect.mOrigin+rect.mEdge1;
          kSegPgm.mDelta = rect.mEdge0;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fS, seg, kSegPgm);
          //              fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fS);
          fT = (Scalar)1.0;
          kPt = seg.mOrigin+seg.mDelta;
          fSqrDist0 = PointRectangleDistanceSq(&fS0, &fT0, kPt, rect);
          //              fSqrDist0 = SqrDistance(kPt,rkRct,&fS0,&fT0);
          fR0 = (Scalar)1.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
      }
      else
      {
        if ( fT < (Scalar)0.0 )  // region 8p
        {
          // min on face s=1 or t=0 or r=1
          kSegPgm.mOrigin = rect.mOrigin+rect.mEdge0;
          kSegPgm.mDelta = rect.mEdge1;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fT, seg, kSegPgm);
          //              fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fT);
          fS = (Scalar)1.0;
          kSegPgm.mOrigin = rect.mOrigin;
          kSegPgm.mDelta = rect.mEdge0;
          fSqrDist0 = SegmentSegmentDistanceSq(&fR0, &fS0, seg, kSegPgm);
          //              fSqrDist0 = SqrDistance(seg,kSegPgm,&fR0,&fS0);
          fT0 = (Scalar)0.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
          kPt = seg.mOrigin+seg.mDelta;
          fSqrDist0 = PointRectangleDistanceSq(&fS0, &fT0, kPt, rect);
          //              fSqrDist0 = SqrDistance(kPt,rkRct,&fS0,&fT0);
          fR0 = (Scalar)1.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
        else if ( fT <= (Scalar)1.0 )  // region 1p
        {
          // min on face s=1 or r=1
          kSegPgm.mOrigin = rect.mOrigin+rect.mEdge0;
          kSegPgm.mDelta = rect.mEdge1;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fT, seg, kSegPgm);
          //              fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fT);
          fS = (Scalar)1.0;
          kPt = seg.mOrigin+seg.mDelta;
          fSqrDist0 = PointRectangleDistanceSq(&fS0, &fT0, kPt, rect);
          //              fSqrDist0 = SqrDistance(kPt,rkRct,&fS0,&fT0);
          fR0 = (Scalar)1.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
        else  // region 2p
        {
          // min on face s=1 or t=1 or r=1
          kSegPgm.mOrigin = rect.mOrigin+rect.mEdge0;
          kSegPgm.mDelta = rect.mEdge1;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fT, seg, kSegPgm);
          //              fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fT);
          fS = (Scalar)1.0;
          kSegPgm.mOrigin = rect.mOrigin+rect.mEdge1;
          kSegPgm.mDelta = rect.mEdge0;
          fSqrDist0 = SegmentSegmentDistanceSq(&fR0, &fS0, seg, kSegPgm);
          //              fSqrDist0 = SqrDistance(seg,kSegPgm,&fR0,&fS0);
          fT0 = (Scalar)1.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
          kPt = seg.mOrigin+seg.mDelta;
          fSqrDist0 = PointRectangleDistanceSq(&fS0, &fT, kPt, rect);
          //              fSqrDist0 = SqrDistance(kPt,rkRct,&fS0,&fT0);
          fR0 = (Scalar)1.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
      }
    }
  }
  else
  {
    // segment and rectangle are parallel
    kSegPgm.mOrigin = rect.mOrigin;
    kSegPgm.mDelta = rect.mEdge0;
    fSqrDist = SegmentSegmentDistanceSq(&fR, &fS, seg, kSegPgm);
    //      fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fS);
    fT = (Scalar)0.0;

    kSegPgm.mDelta = rect.mEdge1;
    fSqrDist0 = SegmentSegmentDistanceSq(&fR0, &fT0, seg, kSegPgm);
    //      fSqrDist0 = SqrDistance(seg,kSegPgm,&fR0,&fT0);
    fS0 = (Scalar)0.0;
    if ( fSqrDist0 < fSqrDist )
    {
      fSqrDist = fSqrDist0;
      fR = fR0;
      fS = fS0;
      fT = fT0;
    }

    kSegPgm.mOrigin = rect.mOrigin+rect.mEdge1;
    kSegPgm.mDelta = rect.mEdge0;
    fSqrDist0 = SegmentSegmentDistanceSq(&fR0, &fS0, seg, kSegPgm);
    //      fSqrDist0 = SqrDistance(seg,kSegPgm,&fR0,&fS0);
    fT0 = (Scalar)1.0;
    if ( fSqrDist0 < fSqrDist )
    {
      fSqrDist = fSqrDist0;
      fR = fR0;
      fS = fS0;
      fT = fT0;
    }

    kSegPgm.mOrigin = rect.mOrigin+rect.mEdge0;
    kSegPgm.mDelta = rect.mEdge1;
    fSqrDist0 = SegmentSegmentDistanceSq(&fR0, &fT0, seg, kSegPgm);
    //      fSqrDist0 = SqrDistance(seg,kSegPgm,&fR0,&fT0);
    fS0 = (Scalar)1.0;
    if ( fSqrDist0 < fSqrDist )
    {
      fSqrDist = fSqrDist0;
      fR = fR0;
      fS = fS0;
      fT = fT0;
    }

    fSqrDist0 = PointRectangleDistanceSq(&fS0, &fT0, seg.mOrigin, rect);
    //      fSqrDist0 = SqrDistance(seg.mOrigin,rkRct,&fS0,&fT0);
    fR0 = (Scalar)0.0;
    if ( fSqrDist0 < fSqrDist )
    {
      fSqrDist = fSqrDist0;
      fR = fR0;
      fS = fS0;
      fT = fT0;
    }

    kPt = seg.mOrigin+seg.mDelta;
    fSqrDist0 = PointRectangleDistanceSq(&fS0, &fT0, kPt, rect);
    //      fSqrDist0 = SqrDistance(kPt,rkRct,&fS0,&fT0);
    fR0 = (Scalar)1.0;
    if ( fSqrDist0 < fSqrDist )
    {
      fSqrDist = fSqrDist0;
      fR = fR0;
      fS = fS0;
      fT = fT0;
    }
  }

  if ( pfSegP )
    *pfSegP = fR;

  if ( pfRctP0 )
    *pfRctP0 = fS;

  if ( pfRctP1 )
    *pfRctP1 = fT;

  return Abs(fSqrDist);
}
*/
//========================================================
// SqrDistance 
//========================================================
Scalar SqrDistance (const Vector3& rkPoint, const tBox& rkBox,
                     Scalar* pfBParam0, Scalar* pfBParam1, Scalar* pfBParam2)
{
  // compute coordinates of point in box coordinate system
  Vector3 kDiff = rkPoint - rkBox.GetCentre();
  Vector3 kClosest(Vec3Dot(kDiff, rkBox.GetOrient()(0)), 
                    Vec3Dot(kDiff, rkBox.GetOrient()(1)),
                    Vec3Dot(kDiff, rkBox.GetOrient()(2)));

  // project test point onto box
  Scalar fSqrDistance = (Scalar)0.0;
  Scalar fDelta;

  if ( kClosest.x  < -rkBox.GetHalfSideLengths()[0] )
  {
    fDelta = kClosest.x  + rkBox.GetHalfSideLengths()[0];
    fSqrDistance += fDelta*fDelta;
    kClosest.x  = -rkBox.GetHalfSideLengths()[0];
  }
  else if ( kClosest.x  > rkBox.GetHalfSideLengths()[0] )
  {
    fDelta = kClosest.x  - rkBox.GetHalfSideLengths()[0];
    fSqrDistance += fDelta*fDelta;
    kClosest.x  = rkBox.GetHalfSideLengths()[0];
  }

  if ( kClosest.y  < -rkBox.GetHalfSideLengths()[1] )
  {
    fDelta = kClosest.y  + rkBox.GetHalfSideLengths()[1];
    fSqrDistance += fDelta*fDelta;
    kClosest.y  = -rkBox.GetHalfSideLengths()[1];
  }
  else if ( kClosest.y  > rkBox.GetHalfSideLengths()[1] )
  {
    fDelta = kClosest.y  - rkBox.GetHalfSideLengths()[1];
    fSqrDistance += fDelta*fDelta;
    kClosest.y  = rkBox.GetHalfSideLengths()[1];
  }

  if ( kClosest.z  < -rkBox.GetHalfSideLengths()[2] )
  {
    fDelta = kClosest.z  + rkBox.GetHalfSideLengths()[2];
    fSqrDistance += fDelta*fDelta;
    kClosest.z  = -rkBox.GetHalfSideLengths()[2];
  }
  else if ( kClosest.z  > rkBox.GetHalfSideLengths()[2] )
  {
    fDelta = kClosest.z  - rkBox.GetHalfSideLengths()[2];
    fSqrDistance += fDelta*fDelta;
    kClosest.z  = rkBox.GetHalfSideLengths()[2];
  }

  if ( pfBParam0 )
    *pfBParam0 = kClosest.x ;

  if ( pfBParam1 )
    *pfBParam1 = kClosest.y ;

  if ( pfBParam2 )
    *pfBParam2 = kClosest.z ;

  return fSqrDistance;
}

//========================================================
// Face 
//========================================================
static void Face(int i0, int i1, int i2, Vector3& rkPnt,
                 const Vector3& rkDir, const tBox& rkBox,
                 const Vector3& rkPmE, Scalar* pfLParam, Scalar& rfSqrDistance)
{
  Vector3 kPpE;
  Scalar fLSqr, fInv, fTmp, fParam, fT, fDelta;

  kPpE[i1] = rkPnt[i1] + rkBox.GetHalfSideLengths()[i1];
  kPpE[i2] = rkPnt[i2] + rkBox.GetHalfSideLengths()[i2];
  if ( rkDir[i0]*kPpE[i1] >= rkDir[i1]*rkPmE[i0] )
  {
    if ( rkDir[i0]*kPpE[i2] >= rkDir[i2]*rkPmE[i0] )
    {
      // v[i1] >= -e[i1], v[i2] >= -e[i2] (distance = 0)
      if ( pfLParam )
      {
        rkPnt[i0] = rkBox.GetHalfSideLengths()[i0];
        fInv = ((Scalar)1.0)/rkDir[i0];
        rkPnt[i1] -= rkDir[i1]*rkPmE[i0]*fInv;
        rkPnt[i2] -= rkDir[i2]*rkPmE[i0]*fInv;
        *pfLParam = -rkPmE[i0]*fInv;
      }
    }
    else
    {
      // v[i1] >= -e[i1], v[i2] < -e[i2]
      fLSqr = rkDir[i0]*rkDir[i0] + rkDir[i2]*rkDir[i2];
      fTmp = fLSqr*kPpE[i1] - rkDir[i1]*(rkDir[i0]*rkPmE[i0] +
                                         rkDir[i2]*kPpE[i2]);
      if ( fTmp <= ((Scalar)2.0)*fLSqr*rkBox.GetHalfSideLengths()[i1] )
      {
        fT = fTmp/fLSqr;
        fLSqr += rkDir[i1]*rkDir[i1];
        fTmp = kPpE[i1] - fT;
        fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*fTmp +
          rkDir[i2]*kPpE[i2];
        fParam = -fDelta/fLSqr;
        rfSqrDistance += rkPmE[i0]*rkPmE[i0] + fTmp*fTmp +
          kPpE[i2]*kPpE[i2] + fDelta*fParam;

        if ( pfLParam )
        {
          *pfLParam = fParam;
          rkPnt[i0] = rkBox.GetHalfSideLengths()[i0];
          rkPnt[i1] = fT - rkBox.GetHalfSideLengths()[i1];
          rkPnt[i2] = -rkBox.GetHalfSideLengths()[i2];
        }
      }
      else
      {
        fLSqr += rkDir[i1]*rkDir[i1];
        fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*rkPmE[i1] +
          rkDir[i2]*kPpE[i2];
        fParam = -fDelta/fLSqr;
        rfSqrDistance += rkPmE[i0]*rkPmE[i0] + rkPmE[i1]*rkPmE[i1] +
          kPpE[i2]*kPpE[i2] + fDelta*fParam;

        if ( pfLParam )
        {
          *pfLParam = fParam;
          rkPnt[i0] = rkBox.GetHalfSideLengths()[i0];
          rkPnt[i1] = rkBox.GetHalfSideLengths()[i1];
          rkPnt[i2] = -rkBox.GetHalfSideLengths()[i2];
        }
      }
    }
  }
  else
  {
    if ( rkDir[i0]*kPpE[i2] >= rkDir[i2]*rkPmE[i0] )
    {
      // v[i1] < -e[i1], v[i2] >= -e[i2]
      fLSqr = rkDir[i0]*rkDir[i0] + rkDir[i1]*rkDir[i1];
      fTmp = fLSqr*kPpE[i2] - rkDir[i2]*(rkDir[i0]*rkPmE[i0] +
                                         rkDir[i1]*kPpE[i1]);
      if ( fTmp <= ((Scalar)2.0)*fLSqr*rkBox.GetHalfSideLengths()[i2] )
      {
        fT = fTmp/fLSqr;
        fLSqr += rkDir[i2]*rkDir[i2];
        fTmp = kPpE[i2] - fT;
        fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*kPpE[i1] +
          rkDir[i2]*fTmp;
        fParam = -fDelta/fLSqr;
        rfSqrDistance += rkPmE[i0]*rkPmE[i0] + kPpE[i1]*kPpE[i1] +
          fTmp*fTmp + fDelta*fParam;

        if ( pfLParam )
        {
          *pfLParam = fParam;
          rkPnt[i0] = rkBox.GetHalfSideLengths()[i0];
          rkPnt[i1] = -rkBox.GetHalfSideLengths()[i1];
          rkPnt[i2] = fT - rkBox.GetHalfSideLengths()[i2];
        }
      }
      else
      {
        fLSqr += rkDir[i2]*rkDir[i2];
        fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*kPpE[i1] +
          rkDir[i2]*rkPmE[i2];
        fParam = -fDelta/fLSqr;
        rfSqrDistance += rkPmE[i0]*rkPmE[i0] + kPpE[i1]*kPpE[i1] +
          rkPmE[i2]*rkPmE[i2] + fDelta*fParam;

        if ( pfLParam )
        {
          *pfLParam = fParam;
          rkPnt[i0] = rkBox.GetHalfSideLengths()[i0];
          rkPnt[i1] = -rkBox.GetHalfSideLengths()[i1];
          rkPnt[i2] = rkBox.GetHalfSideLengths()[i2];
        }
      }
    }
    else
    {
      // v[i1] < -e[i1], v[i2] < -e[i2]
      fLSqr = rkDir[i0]*rkDir[i0]+rkDir[i2]*rkDir[i2];
      fTmp = fLSqr*kPpE[i1] - rkDir[i1]*(rkDir[i0]*rkPmE[i0] +
                                         rkDir[i2]*kPpE[i2]);
      if ( fTmp >= (Scalar)0.0 )
      {
        // v[i1]-edge is closest
        if ( fTmp <= ((Scalar)2.0)*fLSqr*rkBox.GetHalfSideLengths()[i1] )
        {
          fT = fTmp/fLSqr;
          fLSqr += rkDir[i1]*rkDir[i1];
          fTmp = kPpE[i1] - fT;
          fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*fTmp +
            rkDir[i2]*kPpE[i2];
          fParam = -fDelta/fLSqr;
          rfSqrDistance += rkPmE[i0]*rkPmE[i0] + fTmp*fTmp +
            kPpE[i2]*kPpE[i2] + fDelta*fParam;

          if ( pfLParam )
          {
            *pfLParam = fParam;
            rkPnt[i0] = rkBox.GetHalfSideLengths()[i0];
            rkPnt[i1] = fT - rkBox.GetHalfSideLengths()[i1];
            rkPnt[i2] = -rkBox.GetHalfSideLengths()[i2];
          }
        }
        else
        {
          fLSqr += rkDir[i1]*rkDir[i1];
          fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*rkPmE[i1] +
            rkDir[i2]*kPpE[i2];
          fParam = -fDelta/fLSqr;
          rfSqrDistance += rkPmE[i0]*rkPmE[i0] + rkPmE[i1]*rkPmE[i1]
            + kPpE[i2]*kPpE[i2] + fDelta*fParam;

          if ( pfLParam )
          {
            *pfLParam = fParam;
            rkPnt[i0] = rkBox.GetHalfSideLengths()[i0];
            rkPnt[i1] = rkBox.GetHalfSideLengths()[i1];
            rkPnt[i2] = -rkBox.GetHalfSideLengths()[i2];
          }
        }
        return;
      }

      fLSqr = rkDir[i0]*rkDir[i0] + rkDir[i1]*rkDir[i1];
      fTmp = fLSqr*kPpE[i2] - rkDir[i2]*(rkDir[i0]*rkPmE[i0] +
                                         rkDir[i1]*kPpE[i1]);
      if ( fTmp >= (Scalar)0.0 )
      {
        // v[i2]-edge is closest
        if ( fTmp <= ((Scalar)2.0)*fLSqr*rkBox.GetHalfSideLengths()[i2] )
        {
          fT = fTmp/fLSqr;
          fLSqr += rkDir[i2]*rkDir[i2];
          fTmp = kPpE[i2] - fT;
          fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*kPpE[i1] +
            rkDir[i2]*fTmp;
          fParam = -fDelta/fLSqr;
          rfSqrDistance += rkPmE[i0]*rkPmE[i0] + kPpE[i1]*kPpE[i1] +
            fTmp*fTmp + fDelta*fParam;

          if ( pfLParam )
          {
            *pfLParam = fParam;
            rkPnt[i0] = rkBox.GetHalfSideLengths()[i0];
            rkPnt[i1] = -rkBox.GetHalfSideLengths()[i1];
            rkPnt[i2] = fT - rkBox.GetHalfSideLengths()[i2];
          }
        }
        else
        {
          fLSqr += rkDir[i2]*rkDir[i2];
          fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*kPpE[i1] +
            rkDir[i2]*rkPmE[i2];
          fParam = -fDelta/fLSqr;
          rfSqrDistance += rkPmE[i0]*rkPmE[i0] + kPpE[i1]*kPpE[i1] +
            rkPmE[i2]*rkPmE[i2] + fDelta*fParam;

          if ( pfLParam )
          {
            *pfLParam = fParam;
            rkPnt[i0] = rkBox.GetHalfSideLengths()[i0];
            rkPnt[i1] = -rkBox.GetHalfSideLengths()[i1];
            rkPnt[i2] = rkBox.GetHalfSideLengths()[i2];
          }
        }
        return;
      }

      // (v[i1],v[i2])-corner is closest
      fLSqr += rkDir[i2]*rkDir[i2];
      fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*kPpE[i1] +
        rkDir[i2]*kPpE[i2];
      fParam = -fDelta/fLSqr;
      rfSqrDistance += rkPmE[i0]*rkPmE[i0] + kPpE[i1]*kPpE[i1] +
        kPpE[i2]*kPpE[i2] + fDelta*fParam;

      if ( pfLParam )
      {
        *pfLParam = fParam;
        rkPnt[i0] = rkBox.GetHalfSideLengths()[i0];
        rkPnt[i1] = -rkBox.GetHalfSideLengths()[i1];
        rkPnt[i2] = -rkBox.GetHalfSideLengths()[i2];
      }
    }
  }
}
//----------------------------------------------------------------------------
static void CaseNoZeros (Vector3& rkPnt, const Vector3& rkDir,
                         const tBox& rkBox, Scalar* pfLParam, Scalar& rfSqrDistance)
{
  Vector3 kPmE(
    rkPnt.x  - rkBox.GetHalfSideLengths()[0],
    rkPnt.y  - rkBox.GetHalfSideLengths()[1],
    rkPnt.z  - rkBox.GetHalfSideLengths()[2]);

  Scalar fProdDxPy = rkDir.x *kPmE.y ;
  Scalar fProdDyPx = rkDir.y *kPmE.x ;
  Scalar fProdDzPx, fProdDxPz, fProdDzPy, fProdDyPz;

  if ( fProdDyPx >= fProdDxPy )
  {
    fProdDzPx = rkDir.z *kPmE.x ;
    fProdDxPz = rkDir.x *kPmE.z ;
    if ( fProdDzPx >= fProdDxPz )
    {
      // line intersects x = e0
      Face(0,1,2,rkPnt,rkDir,rkBox,kPmE,pfLParam,rfSqrDistance);
    }
    else
    {
      // line intersects z = e2
      Face(2,0,1,rkPnt,rkDir,rkBox,kPmE,pfLParam,rfSqrDistance);
    }
  }
  else
  {
    fProdDzPy = rkDir.z *kPmE.y ;
    fProdDyPz = rkDir.y *kPmE.z ;
    if ( fProdDzPy >= fProdDyPz )
    {
      // line intersects y = e1
      Face(1,2,0,rkPnt,rkDir,rkBox,kPmE,pfLParam,rfSqrDistance);
    }
    else
    {
      // line intersects z = e2
      Face(2,0,1,rkPnt,rkDir,rkBox,kPmE,pfLParam,rfSqrDistance);
    }
  }
}
//----------------------------------------------------------------------------
static void Case0 (int i0, int i1, int i2, Vector3& rkPnt,
                   const Vector3& rkDir, const tBox& rkBox, Scalar* pfLParam,
                   Scalar& rfSqrDistance)
{
  Scalar fPmE0 = rkPnt[i0] - rkBox.GetHalfSideLengths()[i0];
  Scalar fPmE1 = rkPnt[i1] - rkBox.GetHalfSideLengths()[i1];
  Scalar fProd0 = rkDir[i1]*fPmE0;
  Scalar fProd1 = rkDir[i0]*fPmE1;
  Scalar fDelta, fInvLSqr, fInv;

  if ( fProd0 >= fProd1 )
  {
    // line intersects P[i0] = e[i0]
    rkPnt[i0] = rkBox.GetHalfSideLengths()[i0];

    Scalar fPpE1 = rkPnt[i1] + rkBox.GetHalfSideLengths()[i1];
    fDelta = fProd0 - rkDir[i0]*fPpE1;
    if ( fDelta >= (Scalar)0.0 )
    {
      fInvLSqr = ((Scalar)1.0)/(rkDir[i0]*rkDir[i0]+rkDir[i1]*rkDir[i1]);
      rfSqrDistance += fDelta*fDelta*fInvLSqr;
      if ( pfLParam )
      {
        rkPnt[i1] = -rkBox.GetHalfSideLengths()[i1];
        *pfLParam = -(rkDir[i0]*fPmE0+rkDir[i1]*fPpE1)*fInvLSqr;
      }
    }
    else
    {
      if ( pfLParam )
      {
        fInv = ((Scalar)1.0)/rkDir[i0];
        rkPnt[i1] -= fProd0*fInv;
        *pfLParam = -fPmE0*fInv;
      }
    }
  }
  else
  {
    // line intersects P[i1] = e[i1]
    rkPnt[i1] = rkBox.GetHalfSideLengths()[i1];

    Scalar fPpE0 = rkPnt[i0] + rkBox.GetHalfSideLengths()[i0];
    fDelta = fProd1 - rkDir[i1]*fPpE0;
    if ( fDelta >= (Scalar)0.0 )
    {
      fInvLSqr = ((Scalar)1.0)/(rkDir[i0]*rkDir[i0]+rkDir[i1]*rkDir[i1]);
      rfSqrDistance += fDelta*fDelta*fInvLSqr;
      if ( pfLParam )
      {
        rkPnt[i0] = -rkBox.GetHalfSideLengths()[i0];
        *pfLParam = -(rkDir[i0]*fPpE0+rkDir[i1]*fPmE1)*fInvLSqr;
      }
    }
    else
    {
      if ( pfLParam )
      {
        fInv = ((Scalar)1.0)/rkDir[i1];
        rkPnt[i0] -= fProd1*fInv;
        *pfLParam = -fPmE1*fInv;
      }
    }
  }

  if ( rkPnt[i2] < -rkBox.GetHalfSideLengths()[i2] )
  {
    fDelta = rkPnt[i2] + rkBox.GetHalfSideLengths()[i2];
    rfSqrDistance += fDelta*fDelta;
    rkPnt[i2] = -rkBox.GetHalfSideLengths()[i2];
  }
  else if ( rkPnt[i2] > rkBox.GetHalfSideLengths()[i2] )
  {
    fDelta = rkPnt[i2] - rkBox.GetHalfSideLengths()[i2];
    rfSqrDistance += fDelta*fDelta;
    rkPnt[i2] = rkBox.GetHalfSideLengths()[i2];
  }
}
//----------------------------------------------------------------------------
static void Case00 (int i0, int i1, int i2, Vector3& rkPnt,
                    const Vector3& rkDir, const tBox& rkBox, Scalar* pfLParam,
                    Scalar& rfSqrDistance)
{
  Scalar fDelta;

  if ( pfLParam )
    *pfLParam = (rkBox.GetHalfSideLengths()[i0] - rkPnt[i0])/rkDir[i0];

  rkPnt[i0] = rkBox.GetHalfSideLengths()[i0];

  if ( rkPnt[i1] < -rkBox.GetHalfSideLengths()[i1] )
  {
    fDelta = rkPnt[i1] + rkBox.GetHalfSideLengths()[i1];
    rfSqrDistance += fDelta*fDelta;
    rkPnt[i1] = -rkBox.GetHalfSideLengths()[i1];
  }
  else if ( rkPnt[i1] > rkBox.GetHalfSideLengths()[i1] )
  {
    fDelta = rkPnt[i1] - rkBox.GetHalfSideLengths()[i1];
    rfSqrDistance += fDelta*fDelta;
    rkPnt[i1] = rkBox.GetHalfSideLengths()[i1];
  }

  if ( rkPnt[i2] < -rkBox.GetHalfSideLengths()[i2] )
  {
    fDelta = rkPnt[i2] + rkBox.GetHalfSideLengths()[i2];
    rfSqrDistance += fDelta*fDelta;
    rkPnt[i2] = -rkBox.GetHalfSideLengths()[i2];
  }
  else if ( rkPnt[i2] > rkBox.GetHalfSideLengths()[i2] )
  {
    fDelta = rkPnt[i2] - rkBox.GetHalfSideLengths()[i2];
    rfSqrDistance += fDelta*fDelta;
    rkPnt[i2] = rkBox.GetHalfSideLengths()[i2];
  }
}
//----------------------------------------------------------------------------
static void Case000 (Vector3& rkPnt, const tBox& rkBox,
                     Scalar& rfSqrDistance)
{
  Scalar fDelta;

  if ( rkPnt.x  < -rkBox.GetHalfSideLengths()[0] )
  {
    fDelta = rkPnt.x  + rkBox.GetHalfSideLengths()[0];
    rfSqrDistance += fDelta*fDelta;
    rkPnt.x  = -rkBox.GetHalfSideLengths()[0];
  }
  else if ( rkPnt.x  > rkBox.GetHalfSideLengths()[0] )
  {
    fDelta = rkPnt.x  - rkBox.GetHalfSideLengths()[0];
    rfSqrDistance += fDelta*fDelta;
    rkPnt.x  = rkBox.GetHalfSideLengths()[0];
  }

  if ( rkPnt.y  < -rkBox.GetHalfSideLengths()[1] )
  {
    fDelta = rkPnt.y  + rkBox.GetHalfSideLengths()[1];
    rfSqrDistance += fDelta*fDelta;
    rkPnt.y  = -rkBox.GetHalfSideLengths()[1];
  }
  else if ( rkPnt.y  > rkBox.GetHalfSideLengths()[1] )
  {
    fDelta = rkPnt.y  - rkBox.GetHalfSideLengths()[1];
    rfSqrDistance += fDelta*fDelta;
    rkPnt.y  = rkBox.GetHalfSideLengths()[1];
  }

  if ( rkPnt.z  < -rkBox.GetHalfSideLengths()[2] )
  {
    fDelta = rkPnt.z  + rkBox.GetHalfSideLengths()[2];
    rfSqrDistance += fDelta*fDelta;
    rkPnt.z  = -rkBox.GetHalfSideLengths()[2];
  }
  else if ( rkPnt.z  > rkBox.GetHalfSideLengths()[2] )
  {
    fDelta = rkPnt.z  - rkBox.GetHalfSideLengths()[2];
    rfSqrDistance += fDelta*fDelta;
    rkPnt.z  = rkBox.GetHalfSideLengths()[2];
  }
}



//========================================================
// SqrDistance 
//========================================================
Scalar SqrDistance(const tLine& rkLine, const tBox& rkBox,
                    Scalar* pfLParam, Scalar* pfBParam0, Scalar* pfBParam1, Scalar* pfBParam2)
{
  // compute coordinates of line in box coordinate system
  Vector3 kDiff = rkLine.mOrigin - rkBox.GetCentre();
  Vector3 kPnt(Vec3Dot(kDiff, rkBox.GetOrient()(0)),
                Vec3Dot(kDiff, rkBox.GetOrient()(1)),
                Vec3Dot(kDiff, rkBox.GetOrient()(2)));
  Vector3 kDir(Vec3Dot(rkLine.mDir, rkBox.GetOrient()(0)),
                Vec3Dot(rkLine.mDir, rkBox.GetOrient()(1)),
                Vec3Dot(rkLine.mDir, rkBox.GetOrient()(2)));

  // Apply reflections so that direction vector has nonnegative components.
  bool bReflect[3];
  int i;
  for (i = 0; i < 3; i++)
  {
    if ( kDir[i] < (Scalar)0.0 )
    {
      kPnt[i] = -kPnt[i];
      kDir[i] = -kDir[i];
      bReflect[i] = true;
    }
    else
    {
      bReflect[i] = false;
    }
  }

  Scalar fSqrDistance = (Scalar)0.0;

  if ( kDir.x  > (Scalar)0.0 )
  {
    if ( kDir.y  > (Scalar)0.0 )
    {
      if ( kDir.z  > (Scalar)0.0 )
      {
        // (+,+,+)
        CaseNoZeros(kPnt,kDir,rkBox,pfLParam,fSqrDistance);
      }
      else
      {
        // (+,+,0)
        Case0(0,1,2,kPnt,kDir,rkBox,pfLParam,fSqrDistance);
      }
    }
    else
    {
      if ( kDir.z  > (Scalar)0.0 )
      {
        // (+,0,+)
        Case0(0,2,1,kPnt,kDir,rkBox,pfLParam,fSqrDistance);
      }
      else
      {
        // (+,0,0)
        Case00(0,1,2,kPnt,kDir,rkBox,pfLParam,fSqrDistance);
      }
    }
  }
  else
  {
    if ( kDir.y  > (Scalar)0.0 )
    {
      if ( kDir.z  > (Scalar)0.0 )
      {
        // (0,+,+)
        Case0(1,2,0,kPnt,kDir,rkBox,pfLParam,fSqrDistance);
      }
      else
      {
        // (0,+,0)
        Case00(1,0,2,kPnt,kDir,rkBox,pfLParam,fSqrDistance);
      }
    }
    else
    {
      if ( kDir.z  > (Scalar)0.0 )
      {
        // (0,0,+)
        Case00(2,0,1,kPnt,kDir,rkBox,pfLParam,fSqrDistance);
      }
      else
      {
        // (0,0,0)
        Case000(kPnt,rkBox,fSqrDistance);
        if ( pfLParam )
          *pfLParam = (Scalar)0.0;
      }
    }
  }

  // undo reflections
  for (i = 0; i < 3; i++)
  {
    if ( bReflect[i] )
      kPnt[i] = -kPnt[i];
  }

  if ( pfBParam0 )
    *pfBParam0 = kPnt.x;	// kPnt.x ;

  if ( pfBParam1 )
    *pfBParam1 = kPnt.y;	// kPnt.y ;

  if ( pfBParam2 )
    *pfBParam2 = kPnt.z;	// kPnt.z ;

  return fSqrDistance;
}

//========================================================
// SegmentBoxDistanceSq
//========================================================
Scalar JigLib::SegmentBoxDistanceSq(Scalar * pfLParam, 
                                     Scalar * pfBParam0, Scalar * pfBParam1, Scalar * pfBParam2,
                                     const tSegment & rkSeg, const tBox & rkBox)
{
  tLine kLine;
  kLine.mOrigin = rkSeg.mOrigin;
  kLine.mDir = rkSeg.mDelta;

  Scalar fLP, fBP0, fBP1, fBP2;
  Scalar fSqrDistance = SqrDistance(kLine,rkBox,&fLP,&fBP0,&fBP1,&fBP2);
  if ( fLP >= (Scalar)0.0 )
  {
    if ( fLP <= (Scalar)1.0 )
    {
      if ( pfLParam )
        *pfLParam = fLP;

      if ( pfBParam0 )
        *pfBParam0 = fBP0;// + 0.5f;

      if ( pfBParam1 )
        *pfBParam1 = fBP1;// + 0.5f;

      if ( pfBParam2 )
        *pfBParam2 = fBP2;// + 0.5f;

      return fSqrDistance;
    }
    else
    {
      fSqrDistance = SqrDistance(rkSeg.mOrigin+rkSeg.mDelta,
                                 rkBox,pfBParam0,pfBParam1,pfBParam2);

      if ( pfLParam )
        *pfLParam = (Scalar)1.0;
      /*
        if ( pfBParam0 )
        *pfBParam0 += 0.5f;

        if ( pfBParam1 )
        *pfBParam1 += 0.5f;

        if ( pfBParam2 )
        *pfBParam2 += 0.5f;
        */
      return fSqrDistance;
    }
  }
  else
  {
    fSqrDistance = SqrDistance(rkSeg.mOrigin,rkBox,pfBParam0,pfBParam1,pfBParam2);

    if ( pfLParam )
      *pfLParam = (Scalar)0.0;
    /*
      if ( pfBParam0 )
      *pfBParam0 += 0.5f;

      if ( pfBParam1 )
      *pfBParam1 += 0.5f;

      if ( pfBParam2 )
      *pfBParam2 += 0.5f;
      */
    return fSqrDistance;
  }
}


//==============================================================
// PointTriangleDistanceSq
//==============================================================
Scalar JigLib::PointTriangleDistanceSq(Scalar * pfSParam, Scalar * pfTParam, 
                                        const Vector3 & rkPoint, const CTriangle & rkTri)
{
  Vector3 kDiff = rkTri.GetOrigin() - rkPoint;
//  Scalar fA00 = rkTri.GetEdge0().GetLengthSq();
  Scalar fA00 = Vec3LengthSq( rkTri.GetEdge0() );
  Scalar fA01 = Vec3Dot(rkTri.GetEdge0(), rkTri.GetEdge1());
//  Scalar fA11 = rkTri.GetEdge1().GetLengthSq();
  Scalar fA11 = Vec3LengthSq( rkTri.GetEdge1() );
  Scalar fB0 = Vec3Dot(kDiff, rkTri.GetEdge0());
  Scalar fB1 = Vec3Dot(kDiff, rkTri.GetEdge1());
//  Scalar fC = kDiff.GetLengthSq();
  Scalar fC = Vec3LengthSq( kDiff );
  Scalar fDet = Abs(fA00*fA11-fA01*fA01);
  Scalar fS = fA01*fB1-fA11*fB0;
  Scalar fT = fA01*fB0-fA00*fB1;
  Scalar fSqrDist;

  if ( fS + fT <= fDet )
  {
    if ( fS < (Scalar)0.0 )
    {
      if ( fT < (Scalar)0.0 )  // region 4
      {
        if ( fB0 < (Scalar)0.0 )
        {
          fT = (Scalar)0.0;
          if ( -fB0 >= fA00 )
          {
            fS = (Scalar)1.0;
            fSqrDist = fA00+((Scalar)2.0)*fB0+fC;
          }
          else
          {
            fS = -fB0/fA00;
            fSqrDist = fB0*fS+fC;
          }
        }
        else
        {
          fS = (Scalar)0.0;
          if ( fB1 >= (Scalar)0.0 )
          {
            fT = (Scalar)0.0;
            fSqrDist = fC;
          }
          else if ( -fB1 >= fA11 )
          {
            fT = (Scalar)1.0;
            fSqrDist = fA11+((Scalar)2.0)*fB1+fC;
          }
          else
          {
            fT = -fB1/fA11;
            fSqrDist = fB1*fT+fC;
          }
        }
      }
      else  // region 3
      {
        fS = (Scalar)0.0;
        if ( fB1 >= (Scalar)0.0 )
        {
          fT = (Scalar)0.0;
          fSqrDist = fC;
        }
        else if ( -fB1 >= fA11 )
        {
          fT = (Scalar)1.0;
          fSqrDist = fA11+((Scalar)2.0)*fB1+fC;
        }
        else
        {
          fT = -fB1/fA11;
          fSqrDist = fB1*fT+fC;
        }
      }
    }
    else if ( fT < (Scalar)0.0 )  // region 5
    {
      fT = (Scalar)0.0;
      if ( fB0 >= (Scalar)0.0 )
      {
        fS = (Scalar)0.0;
        fSqrDist = fC;
      }
      else if ( -fB0 >= fA00 )
      {
        fS = (Scalar)1.0;
        fSqrDist = fA00+((Scalar)2.0)*fB0+fC;
      }
      else
      {
        fS = -fB0/fA00;
        fSqrDist = fB0*fS+fC;
      }
    }
    else  // region 0
    {
      // minimum at interior point
      Scalar fInvDet = ((Scalar)1.0)/fDet;
      fS *= fInvDet;
      fT *= fInvDet;
      fSqrDist = fS*(fA00*fS+fA01*fT+((Scalar)2.0)*fB0) +
        fT*(fA01*fS+fA11*fT+((Scalar)2.0)*fB1)+fC;
    }
  }
  else
  {
    Scalar fTmp0, fTmp1, fNumer, fDenom;

    if ( fS < (Scalar)0.0 )  // region 2
    {
      fTmp0 = fA01 + fB0;
      fTmp1 = fA11 + fB1;
      if ( fTmp1 > fTmp0 )
      {
        fNumer = fTmp1 - fTmp0;
        fDenom = fA00-2.0f*fA01+fA11;
        if ( fNumer >= fDenom )
        {
          fS = (Scalar)1.0;
          fT = (Scalar)0.0;
          fSqrDist = fA00+((Scalar)2.0)*fB0+fC;
        }
        else
        {
          fS = fNumer/fDenom;
          fT = (Scalar)1.0 - fS;
          fSqrDist = fS*(fA00*fS+fA01*fT+2.0f*fB0) +
            fT*(fA01*fS+fA11*fT+((Scalar)2.0)*fB1)+fC;
        }
      }
      else
      {
        fS = (Scalar)0.0;
        if ( fTmp1 <= (Scalar)0.0 )
        {
          fT = (Scalar)1.0;
          fSqrDist = fA11+((Scalar)2.0)*fB1+fC;
        }
        else if ( fB1 >= (Scalar)0.0 )
        {
          fT = (Scalar)0.0;
          fSqrDist = fC;
        }
        else
        {
          fT = -fB1/fA11;
          fSqrDist = fB1*fT+fC;
        }
      }
    }
    else if ( fT < (Scalar)0.0 )  // region 6
    {
      fTmp0 = fA01 + fB1;
      fTmp1 = fA00 + fB0;
      if ( fTmp1 > fTmp0 )
      {
        fNumer = fTmp1 - fTmp0;
        fDenom = fA00-((Scalar)2.0)*fA01+fA11;
        if ( fNumer >= fDenom )
        {
          fT = (Scalar)1.0;
          fS = (Scalar)0.0;
          fSqrDist = fA11+((Scalar)2.0)*fB1+fC;
        }
        else
        {
          fT = fNumer/fDenom;
          fS = (Scalar)1.0 - fT;
          fSqrDist = fS*(fA00*fS+fA01*fT+((Scalar)2.0)*fB0) +
            fT*(fA01*fS+fA11*fT+((Scalar)2.0)*fB1)+fC;
        }
      }
      else
      {
        fT = (Scalar)0.0;
        if ( fTmp1 <= (Scalar)0.0 )
        {
          fS = (Scalar)1.0;
          fSqrDist = fA00+((Scalar)2.0)*fB0+fC;
        }
        else if ( fB0 >= (Scalar)0.0 )
        {
          fS = (Scalar)0.0;
          fSqrDist = fC;
        }
        else
        {
          fS = -fB0/fA00;
          fSqrDist = fB0*fS+fC;
        }
      }
    }
    else  // region 1
    {
      fNumer = fA11 + fB1 - fA01 - fB0;
      if ( fNumer <= (Scalar)0.0 )
      {
        fS = (Scalar)0.0;
        fT = (Scalar)1.0;
        fSqrDist = fA11+((Scalar)2.0)*fB1+fC;
      }
      else
      {
        fDenom = fA00-2.0f*fA01+fA11;
        if ( fNumer >= fDenom )
        {
          fS = (Scalar)1.0;
          fT = (Scalar)0.0;
          fSqrDist = fA00+((Scalar)2.0)*fB0+fC;
        }
        else
        {
          fS = fNumer/fDenom;
          fT = (Scalar)1.0 - fS;
          fSqrDist = fS*(fA00*fS+fA01*fT+((Scalar)2.0)*fB0) +
            fT*(fA01*fS+fA11*fT+((Scalar)2.0)*fB1)+fC;
        }
      }
    }
  }

  if ( pfSParam )
    *pfSParam = fS;

  if ( pfTParam )
    *pfTParam = fT;

  return Abs(fSqrDist);
}


//====================================================================
// SegmentTriangleDistanceSq
//====================================================================
Scalar JigLib::SegmentTriangleDistanceSq(Scalar* segT, Scalar* triT0, Scalar* triT1, const tSegment& seg, const CTriangle& triangle)
{
  // compare segment to all three edges of the triangle
//  Scalar distSq = std::numeric_limits<Scalar>::m.x ;
  Scalar distSq = FLT_MAX;

  if (SegmentTriangleOverlap(segT, triT0, triT1, seg, triangle))
  {
    return 0.0f;
  }

  Scalar s, t, u;
  Scalar distEdgeSq;
  distEdgeSq = SegmentSegmentDistanceSq(&s, &t, seg, tSegment(triangle.GetOrigin(), triangle.GetEdge0()));
  if (distEdgeSq < distSq)
  {
    distSq = distEdgeSq;
    if (segT) *segT = s;
    if (triT0) *triT0 = t;
    if (triT1) *triT1 = 0.0f;
  }
  distEdgeSq = SegmentSegmentDistanceSq(&s, &t, seg, tSegment(triangle.GetOrigin(), triangle.GetEdge1()));
  if (distEdgeSq < distSq)
  {
    distSq = distEdgeSq;
    if (segT) *segT = s;
    if (triT0) *triT0 = 0.0f;
    if (triT1) *triT1 = t;
  }
  distEdgeSq = SegmentSegmentDistanceSq(&s, &t, seg, tSegment(triangle.GetOrigin() + triangle.GetEdge0(), triangle.GetEdge2()));
  if (distEdgeSq < distSq)
  {
    distSq = distEdgeSq;
    if (segT) *segT = s;
    if (triT0) *triT0 = 1.0f - t;
    if (triT1) *triT1 = t;
  }

  // compare segment end points to triangle interior
  Scalar startTriSq = PointTriangleDistanceSq(&t, &u, seg.GetOrigin(), triangle);
  if (startTriSq < distSq)
  {
    distSq = startTriSq;
    if (segT) *segT = 0.0f;
    if (triT0) *triT0 = t;
    if (triT1) *triT1 = u;
  }
  Scalar endTriSq = PointTriangleDistanceSq(&t, &u, seg.GetEnd(), triangle);
  if (endTriSq < distSq)
  {
    distSq = endTriSq;
    if (segT) *segT = 1.0f;
    if (triT0) *triT0 = t;
    if (triT1) *triT1 = u;
  }
  return distSq;
}
