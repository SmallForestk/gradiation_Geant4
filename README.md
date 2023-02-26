# gradiation_Geant4
　CNNの学習のために使用するデータセットを作成するためのGeant4のセットアップ。
 `B4a_random`では学習に使用する、入射エネルギーがランダムなシミュレーションデータを作成し、`B4a_stable`ではLinearityやResolutionを見るときに使用する、入射エネルギーが一定なシミュレーションデータを作成する。

## 1.シミュレーション
### 1.1.Geant4のビルド
`B4a_random`と`B4a_stable`の2つを別々のディレクトリで、
```
cmake -DGeant4_DIR=/usr/local/geant4/10.07.p02/lib/Geant4-10.07.p02
```
というcmakeコマンドよりを用いてビルドする。

### 1.2.Geant4を実行するマクロ
`B4a_random`をビルドしたものでは、`pi_random.mac`を用いてシミュレーションを実行する。

`B4a_stable`をビルドしたものでは、`pi_macro`の中にあるマクロファイルを使ってシミュレーションを実行する。
実行するときには`energy.sh`のようにシェルスクリプトを用いて、`pi_macro`のなかのマクロファイルを一つずつ実行しつつ、出力ファイル名の変更とファイルの移動を行うようにした。


## 2.シミュレーションの概要
### 2.1. シミュレーションしているカロリメータ
 `B4a_random`、`B4a_satble`のどちらも、シミュレーションするのはサンプリング型のカロリメータである。
検出層は1cm x 1cmのタイルに分割されている。（CNNで使用する際にはタイルの大きさを3cm x 3cmに変更する）
なお、検出層と吸収層は以下の表の様になっている。
||吸収層|検出層|
|:---:|:---:|:---:|
|マテリアル|G4_Fe|G4_PLASTIC_SC_VINYLTOLUENE|
|厚さ|2cm|3mm|

### 2.2. 打ち込む粒子
 シミュレーションの際には粒子はカロリメータの中心から2m離れた位置で生成され、カロリメータの中心に垂直に入社するようになっている。
入射エネルギーは"B4a_stable"ではマクロファイルから設定する用になっていが、"B4a_random"ではビルド前にgradiation_Geant4/B4a_random/src/B4PrimaryGeneratorAction.cc"の中の
```
111  G4double MaxEnergy = 30;
112  G4double minEnergy = 1;
```
で最大値と最小値を設定する。

### 2.3. 出力ファイル
 出力される`B4.root`の中にはTree形式でシミュレーションしたイベントの情報が保存される様になっており、4つの
Treeが保存される。

 1つ目の`B4`は吸収層、検出層での粒子によるEnergy Depositの合計と粒子の飛行距離がEvent Numberと一緒に保存される様になっている。
 
 2つ目の`Edep`には1EventでEnergy Depositがあった場合にそれが検出層と吸収層のどちらであるのか、検出そうであった場合にはそのタイルの位置と一緒に保存される。
各Branchに保存される値は以下の通りである
|Branch名|保存される値|
|:---:|:---:|
|Enumber|Event番号|
|Lnumber|タイルor吸収層のLayer番号|
|TXnumber|タイルのX方向における番号（吸収層であった場合は０）|
|TYnumber|タイルのY方向における番号（吸収層であった場合は０）|
|GorA|吸収層なら０、検出層なら１が保存される|
|Edep|Energy Deposit|

 ３つ目の`Gap_Edep`は検出層それぞれのタイルでEnergy Depositがあった場合にその時間とタイルの位置をEnergy Depositの値と一緒に保存される。
２つ目の`Edep`と共通するBranch名には同じ変数が保存されており、追加でTimeというBranchにはEnergy　Depositがあった時間、ParticlIDというBranchにはEnergy Depositのもととなった粒子のIDが保存される。
