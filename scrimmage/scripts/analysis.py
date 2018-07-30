# ---------------------------------------------------------------------------
# @section LICENSE
#  
# Copyright (c) 2016 Georgia Tech Research Institute (GTRI) 
#               All Rights Reserved
#  
# The above copyright notice and this permission notice shall be included in 
# all copies or substantial portions of the Software.
#  
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
# DEALINGS IN THE SOFTWARE.
# ---------------------------------------------------------------------------
# @file filename.ext
# @author Kevin DeMarco <kevin.demarco@gtri.gatech.edu> 
# @author Eric Squires <eric.squires@gtri.gatech.edu>
# @version 1.0
# ---------------------------------------------------------------------------
# @brief A brief description.
# 
# @section DESCRIPTION
# A long description.
# ---------------------------------------------------------------------------
import pandas as pd
import patsy
import numpy as np
import lvdb
import statsmodels.api as sm
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.mlab as mlab


def square(x):
    return x**2


def main():

    df = pd.DataFrame(data=np.hstack((np.load('xx.npy'), np.load('yy.npy'))),
                      columns=['greedy_b', 'greedy_r', 'score_b', 'score_r'])

    df = df[df['score_b'] >= 0]
    df['score_diff'] = -df['score_b'] + df['score_r']

    y, X = patsy.dmatrices('score_diff ~ '
                           'greedy_b + '
                           'greedy_r + '
                           'square(greedy_b) + '
                           'square(greedy_r) + '
                           'greedy_b:greedy_r',
                           data=df, return_type='dataframe')
    result = sm.OLS(y, X).fit()
    print result.summary()

    x = df['greedy_b']
    y = df['greedy_r']
    z = df['score_diff']

    '''
    xi = np.linspace(0, 1, 1000)
    yi = np.linspace(0, 1, 1000)
    xx, yy = np.meshgrid(xi, yi)

    zz = mlab.griddata(x, y, z, xi, yi, interp='linear')

    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    surf = ax.plot_surface(xx, yy, zz)
    ax.set_xlabel('blue_pct_greedy')
    ax.set_ylabel('red_pct_greedy')
    ax.set_zlabel('score_diff')
    plt.show()
    '''

    fig, axes = plt.subplots(1, 3)
    df_temp = df[(df['greedy_r'] >= 0) & (df['greedy_r'] < 0.05)]
    axes[0].scatter(df_temp['greedy_b'], df_temp['score_diff'])
    axes[0].set_title('% greedy (red) = 0')
    axes[0].set_ylabel('blue - red score')
    df_temp = df[(df['greedy_r'] > 0.45) & (df['greedy_r'] < 0.55)]
    axes[1].scatter(df_temp['greedy_b'], df_temp['score_diff'])
    axes[1].set_xlabel('% greedy (blue)')
    axes[1].set_title('% greedy (red) = 0.5')
    df_temp = df[(df['greedy_r'] > 0.95) & (df['greedy_r'] < 1.1)]
    axes[2].scatter(df_temp['greedy_b'], df_temp['score_diff'])
    axes[2].set_title('% greedy (red) = 1')
    plt.savefig("dive_bomber_analysis.png")
    plt.show()
    '''
    df = df.sort('dist_start')
    #plt.plot(df['dist_start'], df['time'])
    fig, ax = plt.subplots()
    ax.plot(df['dist_start'], df['time'])
    ax.set_xlabel('behavior start distance (m)')
    ax.set_ylabel('time to shootdown red (s)')

    plt.savefig("starting_dist_vs_time_to_shot.png")
    plt.show()
    '''


if __name__ == '__main__':
    main()
