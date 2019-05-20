%  This function takes a problem in SeDuMi MATLAB format and writes it out 
%  in SDPA sparse format.  
%
%  Usage:
%
%  ret=writesdpa(fname,A,b,c,K,pars)
%
%      fname           Name of SDPpack file, in quotes
%      A,b,c,K         Problem in SeDuMi form
%      pars            Optional parameters.
%                      pars.printlevel=0           No printed output  
%                      pars.printlevel=1 (default) Some printed output.
%      
%      ret             ret=0 on success, ret=1 on failure.
%
%  Notes:
%
%     Problems with complex data are not allowed.    
%
%     Quadratic cone and rotated cone constraints are not supported.  
%
%     Nonsymmetric A.s and C.s matrices are symmetrized with A=(A+A')/2
%     a warning is given when this happens.
%
%     Free variables are not supported.
%
%     Floating point numbers are written out with 18 decimal digits for
%     accuracy.
%
%  Please contact the author (Brian Borchers, borchers@nmt.edu) with any
%  questions or bug reports.
%
%  Third Version: 3/3/06.  Corrected a bug in the handling of
%                          nonsymmetric constraint matrices.
%
%  Second Version: 7/14/04.  Modified to vastly speed up operations on sparse
%                 matrices.  On some problems, this version is 100 times 
%                 faster! 
%
%  First Version: 7/14/03.  Modified from old writesdp.m which wrote problems
%                 in SDPPack format.
%
function ret=writesdpa(fname,A,b,c,K,pars)
%
% First, check to see whether or not we should be quiet.
%
if (nargin > 5)
  if (isfield(pars,'printlevel'))
    if (pars.printlevel == 0)
      quiet=1;
    else
      quiet=0;
    end
  else
    quiet=0;
  end
else
  pars.printlevel=1;
  quiet=0;
end
%
%  First, check for complex numbers in A, b, or c.
%
if (isreal(A) ~= 1)
  if (quiet == 0)
    fprintf('A is not real!\n');
  end
  ret=1;
  return
end
if (isreal(b) ~= 1)
  if (quiet == 0)
    fprintf('b is not real!\n');
  end
  ret=1;
  return
end
if (isreal(c) ~= 1)
  if (quiet == 0)
    fprintf('c is not real!\n');
  end
  ret=1;
  return
end
%
%  Check for any quadratic cone constraints.
%
if (isfield(K,'q'))
  if ((~isempty(K.q)) & (K.q ~= 0))
    if (quiet == 0)
      fprintf('quadratic cone constraints are not supported.\n');
    end
    ret=1;
    return
  end
end 
%
%  Check for any rotated cone constraints.
%
if (isfield(K,'r'))
  if ((~isempty(K.r)) & (K.r ~= 0))
    if (quiet == 0)
      fprintf('rotated cone constraints are not supported.\n');
    end
    ret=1;
    return
  end
end 
%
% Check for any free variables.
%
if (isfield(K,'f'))
  if ((~isempty(K.f)) & (K.f ~= 0))
    if (quiet == 0)
      fprintf('Free variables are not supported.\n');
    end
    ret=1;
    return
  end
end 
%
%  Find the number of constraints.
%
m=length(b);
%
%  Deal with the following special case.  If A is transposed, transpose
%  it again so that it is of the right size.
%
[Am,An]=size(A);
if (Am ~= m)
  if (An == m)
    if (quiet==0)  
      fprintf('Transposing A to match b \n');
    end
    AT=A;
    A=A';
%
% Also swap Am and An so that they're correct.
%
    temp=Am;
    Am=An;
    An=temp;
  else
%
% In this case, A is just plain the wrong size.
%
    if (quiet==0)
      fprintf('A is not of the correct size to match b \n');
    end
    ret=1;
    return
  end
else
%
% No need to transpose A, but we'll need AT.
%
  AT=A';
end
%
%  Deal with the following special case:  if c==0, then c should really
%  be a zero vector of the appropriate size.
%
if (c == 0)
  if (quiet==0)
    fprintf('Expanding c to the appropriate size\n');
  end
  c=sparse(An,1);
end
%
% If c is empty, then act as if it was zero.
%
if (isempty(c))
  if (quiet==0)
    fprintf('Expanding empty c to zeros of the appropriate size\n');
  end
  c=sparse(An,1);
end
%
% If c is a row vector, make it a column vector.
%
[cm,cn]=size(c);
if (cn > cm)
  c=c';
end
%
%  Get the size data.
%
%
% First, the size of the LP block.
%
if (isfield(K,'l'))
  nlin=K.l;
  sizelin=nlin;
  if (isempty(sizelin))
    sizelin=0;
    nlin=0;
  end
  if (K.l == 0)
    nlin=0;
    sizelin=0;
  end
else
  nlin=0;
  sizelin=0;
end
%
% Get the sizes of the SDP blocks.
%
if (isfield(K,'s'))
  nsdpblocks=length(K.s);
  sizesdp=sum((K.s).^2);
  if (isempty(sizesdp))
    sizesdp=0;
    nsdpblocks=0;
  end
  if (K.s == 0)
    nsdpblocks=0;
    sizesdp=0;
  end
else
  sizesdp=0;
  nsdpblocks=0;
end
%
% Figure out the number of blocks.
%
nblocks=nsdpblocks;
if (nlin>0)
  nblocks=nblocks+1;
end
%
%  print out some size information
%
if (quiet==0)
  fprintf('Number of constraints: %d \n',m);
  fprintf('Number of SDP blocks: %d \n',nsdpblocks);
  fprintf('Number of LP vars: %d \n',nlin);
end
%
%  Open up the file for writing.
%
fid=fopen(fname,'w');
if (fid==-1)
  if (quiet==0)
    fprintf('Could not open file for output!');
  end
  ret=1;
  return
end
%
%  Print out m, the number of constraints.
%
fprintf(fid,'%d \n',m);
%
% Next, the number of blocks.
%
fprintf(fid,'%d \n',nblocks);
%
% Print out the block structure.
%
if (K.s > 0)
  fprintf(fid,'%d ',K.s);
end
if (nlin > 0)
  fprintf(fid,'%d ',-nlin);
end
fprintf(fid,'\n');
%
%  Next, b, with all on one line.
%
fprintf(fid,'%.18e  ',full(b));
fprintf(fid,'\n');
%
% First, the C matrix.
%
%
%  First, calculate where in c things start.
%
base=sizelin+1;
%
%  Next, work through the SDP blocks.
%
for i=1:nsdpblocks
%
% Get the current block of C.
%
  I=find(c);
  II=find(I>=base);
  I=I(II);
  II=find(I<=base+K.s(i)^2-1);
  I=I(II);
  II=I-(base-1)*ones(size(I));
  work=sparse(II,ones(size(II)),c(I),K.s(i)^2,1);
  work=reshape(work,K.s(i),K.s(i));

%
% Check this block for symmetry.
%
if (norm(work-work','fro') ~= 0)
  if (quiet==0)
    fprintf('Non symmetric C.s matrix!\n');
  end
  work=(work+work')/2;
end
% 
% Write out the C.s matrix. 
%
  work=triu(work);
  [II,JJ,V]=find(work);
  cnt=length(II);
  if (cnt ~= 0)
    fprintf(fid,'%d %d %d %d %.18e\n',[zeros(size(II)) i*ones(size(II)) II JJ -V]');
  end

%
%  Next, update to the next base.
%
  base=base+K.s(i)^2;
end
%
% Print out the coefficients for the linear block of C.
%
for i=1:nlin
  if (c(i) ~= 0.0)
    fprintf(fid,'%d %d %d %d %.18e\n',[0 nsdpblocks+1 i i -c(i)]);
  end
end

%
%  Now, loop through the constraints, one at a time.
%
for cn=1:m
%
%  Print out the SDP part of constraint cn.
%
  base=sizelin+1;
  rowcn=AT(:,cn);
  for i=1:nsdpblocks
    I=find(rowcn);
    II=find(I>=base);
    I=I(II);
    II=find(I<=(base+K.s(i)^2-1));
    I=I(II);
    II=I-(base-1)*ones(size(I));
    work=sparse(II,ones(size(II)),rowcn(I),K.s(i)^2,1);

    work=reshape(work,K.s(i),K.s(i));

    if (norm(work-work','fro') ~= 0)
      if (quiet==0)
        fprintf('Non symmetric A.s matrix! \n');
      end
      work=(work+work')/2;
    end
%
% Ignore the lower left triangle.
%
    work=triu(work);
%
% Get the nonzero entries.
%
    [II,JJ,V]=find(work);
    cnt=length(II);
    if (cnt ~= 0)
      fprintf(fid,'%d %d %d %d %.18e\n',[cn*ones(size(II)) i*ones(size(II)) II JJ V]');
    end
%
%  Next, update to the next base.
%
    base=base+K.s(i)^2;
  end
%
% Finally, the linear part.
%
  I=find(rowcn);
  II=find(I<=nlin);
  I=I(II);
  workrow=sparse(I,ones(size(I)),rowcn(I),nlin,1);
  [II,JJ,V]=find(workrow);
  if (length(II) > 0)
    fprintf(fid,'%d %d %d %d %.18e\n',[cn*ones(length(II),1) (nsdpblocks+1)*ones(length(II),1) II II V]');
  end
end
%
% Close the file.
%
fclose(fid);
%
% Return success
%
ret=0;
return
