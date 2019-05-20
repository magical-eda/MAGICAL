%
% [A,b,c,K]=convertf(A,b,c,K)
%
% converts free variables in a SeDuMi problem into nonnegative LP variables.
%
function [A,b,c,K]=convertf(A,b,c,K)
%
% Get the number of constraints.
%
m=length(b);
%
%  Deal with the following special case.  If A is transposed, transpose
%  it again so that it is of the right size.
%
[Am,An]=size(A);
if (Am ~= m)
  if (An == m)
    fprintf('Transposing A to match b \n');
    A=A';
  else
    fprintf('A is not of the correct size to match b \n');
    return
  end
end
%
%  Deal with the following special case:  if c==0, then c should really
%  be a zero vector of the appropriate size.
%
if (c == 0)
  fprintf('Expanding c to the appropriate size\n');
  [Am,An]=size(A);
  c=zeros(An,1);
end
%
% If c is empty, then act as if it was zero.
%
if (isempty(c))
  fprintf('Expanding empty c to zeros of the appropriate size\n');
  [Am,An]=size(A);
  c=zeros(An,1);
end
%
% If c is a row vector, make it a column vector.
%
[cm,cn]=size(c);
if (cn > cm)
  c=c';
end
%
% Check for any free LP variables and rewrite them as the differences of
% regular LP variables.
%
if (isfield(K,'f'))
  nfree=K.f
  fprintf('Converting %d free variables to LP variables\n',nfree);
  if (isfield(K,'l'))
    nlin=K.l;
  else
    nlin=0;
  end
  [Am,An]=size(A);
  Anew=[A(:,1:nfree) -A(:,1:nfree) A(:,nfree+1:An)];
  A=Anew;
  cnew=[c(1:nfree); -c(1:nfree); c(nfree+1:An)];
  c=cnew;

  K.l=nlin+2*nfree;
  K.f=0;
end


