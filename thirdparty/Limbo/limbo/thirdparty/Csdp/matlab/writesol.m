%
% writesol(fname,x,y,z,K)
%
% Writes out a solution file in the format used by CSDP and
% readsol.
%
%   fname     File name to read solution from.
%   x,y,z     Solution.
%   K         structure of the matrices.
%
% 
%
function ret=writesol(fname,x,y,z,K);
%
%  First, eliminate special cases that we don't handle.
%
%
%  Check for any quadratic cone constraints.
%
if (isfield(K,'q') & (~isempty(K.q)) & (K.q ~= 0)),
  fprintf('quadratic cone constraints are not supported.\n');
  ret=100;
  return
end 
%
%  Check for any rotated cone constraints.
%
if (isfield(K,'r') & (~isempty(K.r)) & (K.r ~= 0)),
  fprintf('rotated cone constraints are not supported.\n');
  ret=100;
  return
end 
%
% Check for any free variables.
%
if (isfield(K,'f') & (~isempty(K.f)) & (K.f ~= 0)),
  fprintf('Free variables are not supported.\n');
  ret=100;
  return
end 
%
% Figure out the m dimension.
%
m=length(y);
%
% Figure out the structure of the LP and SDP blocks.
%
if (isfield(K,'l')),
  if (K.l > 0)
    nlin=K.l;
  else
    K.l=0;
    nlin=0;
  end
else
  K.l=0;
  nlin=0;
end
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
      end
    else
      nsdpblocks=0;
      K.s=[]; 
    end
  end
else
  K.s=[];
  nsdpblocks=0;
end

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
end
veclpbase=1;
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
end 
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
  end
end
%
%  Open up the file.
%
fid=fopen(fname,'w');
if (fid == -1),
  fprintf('file open failed!\n');
  ret=100;
  return
end
%
% Write out y.
%
for i=1:m
  fprintf(fid,'%.18e ',-y(i));
end
fprintf(fid,'\n');
%
% Entries of Z.
%
%
% First, the SDP blocks.
%
for i=1:nsdpblocks
  base=vecsdpbase(i);
  tempmat=reshape(z(base:base+K.s(i)^2-1),K.s(i),K.s(i));
  for j=1:K.s(i)
    for k=j:K.s(i)
      if (tempmat(j,k) ~= 0)
	fprintf(fid,'1 %d %d %d %.18e \n',[i,j,k,tempmat(j,k)]);
      end
    end
  end
end
%
% The linear block is last.
%
if (nlin > 0)
  for i=veclpbase:nlin
    if (x(i) ~= 0)
      fprintf(fid,'1 %d %d %d %.18e \n',[nsdpblocks+1 i-veclpbase+1 ...
		    i-veclpbase+1 z(i)]);
    end
  end
end
%
% Entries of X.
%
%
% First, the SDP blocks.
%
for i=1:nsdpblocks
  base=vecsdpbase(i);
  tempmat=reshape(x(base:base+K.s(i)^2-1),K.s(i),K.s(i));
  for j=1:K.s(i)
    for k=j:K.s(i)
      if (tempmat(j,k) ~= 0)
	fprintf(fid,'2 %d %d %d %.18e \n',[i,j,k,tempmat(j,k)]);
      end
    end
  end
end
%
% The linear block is last.
%
if (nlin > 0)
  for i=veclpbase:nlin
    if (x(i) ~= 0)
      fprintf(fid,'2 %d %d %d %.18e \n',[nsdpblocks+1 i-veclpbase+1 ...
		    i-veclpbase+1 x(i)]);
    end
  end
end


%
% Close the output file and return.
%
fclose(fid);
ret=0;
