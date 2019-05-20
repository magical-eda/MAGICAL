%
% [x,y,z]=readsol(fname,K,m)
%
%   fname     File name to read solution from.
%   K         structure of the matrices.
%   m         size of y vector.
% 
% Modified 7/15/04, for greater MATLAB acceleration.
%
function [x,y,z]=readsol(fname,K,m)
%
%  First, eliminate special cases that we don't handle.
%
%
%  Check for any quadratic cone constraints.
%
if (isfield(K,'q') & (~isempty(K.q)) & (K.q ~= 0)),
  fprintf('quadratic cone constraints are not supported.\n');
  return;
end; 
%
%  Check for any rotated cone constraints.
%
if (isfield(K,'r') & (~isempty(K.r)) & (K.r ~= 0)),
  fprintf('rotated cone constraints are not supported.\n');
  return;
end; 
%
% Check for any free variables.
%
if (isfield(K,'f') & (~isempty(K.f)) & (K.f ~= 0)),
  fprintf('Free variables are not supported.\n');
  return;
end; 
%
% Figure out the structure of the LP and SDP blocks.
%
if (isfield(K,'l')),
  if (K.l > 0)
    nlin=K.l;
  else
    K.l=0;
    nlin=0;
  end;
else
  K.l=0;
  nlin=0;
end;
%
% Patched on 10/23/03 to handle all kinds of stupid ways of indicating
% no SDP block.
%
if (isfield(K,'s')),
  if (length(K.s) > 1),
    nsdpblocks=length(K.s);
  else
    if (length(K.s)==1),
      if (K.s==0)
        nsdpblocks=0;
        K.s=[];
      else
        nsdpblocks=1;
      end;
    else
      nsdpblocks=0;
      K.s=[]; 
    end;
  end;
else
  K.s=[];
  nsdpblocks=0;
end;

%
% First, where everything is in the vector.
%
% vecsdpbase(i)=point in vector at which SDP block i starts.
% v(1..nlin)         LP variables.
%
base=nlin+1;
for i=1:length(K.s),
  vecsdpbase(i)=base;
  base=base+(K.s(i))^2;
end;

%
% Second, where everything is in the matrix.
%
% matsdpbase(i)=   index of upper left corner of SDP block i.
% matlpbase        index of start of LP block.
%
base=1;
for i=1:length(K.s),
  matsdpbase(i)=base;
  base=base+K.s(i);
end; 
matlpbase=base;
%
% Setup an array containing blocksizes. blocksize(i) is used as a faster
% synonym for K.s(i) in what follows.  This is because MATLAB doesn't
% accelerate statements involving fields.  
%
if (nsdpblocks >= 1),
  blocksizes=zeros(nsdpblocks,1);
  for i=1:nsdpblocks,
    blocksizes(i)=K.s(i);
  end;
end;
%
%  Open up the file.
%
fid=fopen(fname,'r');
if (fid == -1),
  fprintf('file does not exist!\n');
  x=NaN;
  y=NaN;
  z=NaN;
  return;
end;
%
% Read y.
%
y=fscanf(fid,'%le',m);
%
% Read the remaining entries.
%
[A,count]=fscanf(fid,'%d %d %d %d %le',[5,inf]);
count=count/5;
%
% Allocate storage for x and z.
%
if ((length(K.s) > 1) | (length(K.s==1) & (K.s>0))),
  veclength=vecsdpbase(length(K.s))+K.s(nsdpblocks)^2-1;
else
  veclength=nlin;
end;
%
% Allocate space for x and z.  We could use sparse vectors here, but 
% the dense vector is vastly faster.
%
x=zeros(veclength,1);
z=zeros(veclength,1);
%
% now, loop through the entries and put them into x and z.
%
for i=1:count,
  if (A(1,i)==1),
%
% A z entry.
%    
    blk=A(2,i);
    indexi=A(3,i);
    indexj=A(4,i);
    ent=A(5,i);

    if (blk==nsdpblocks+1)
      z(indexi)=ent;
    else
%
% In one of the SDP blocks.
%      
%      [blk, indexi, indexj, K.s(blk)]

      z(vecsdpbase(blk)+indexi+(indexj-1)*blocksizes(blk)-1)=ent;
      z(vecsdpbase(blk)+indexj+(indexi-1)*blocksizes(blk)-1)=ent;
    end;
  else
%
% An x entry.
%    
    blk=A(2,i);
    indexi=A(3,i);
    indexj=A(4,i);
    ent=A(5,i);

    if (blk==nsdpblocks+1)
      x(indexi)=ent;
    else
%
% In one of the SDP blocks.
%      
      x(vecsdpbase(blk)+indexi+(indexj-1)*blocksizes(blk)-1)=ent;
      x(vecsdpbase(blk)+indexj+(indexi-1)*blocksizes(blk)-1)=ent;
    end;
  end;
end;
%
% Correction for the difference between CSDP and SeDuMi primal/dual pair.
%
y=-y;
%
% close the file.  
%
fclose(fid);
