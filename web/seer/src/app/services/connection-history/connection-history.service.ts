import { Injectable } from '@angular/core';
import { LocalStorageService } from 'angular-2-local-storage';
import { Observable } from 'rxjs/Observable';
import { ReplaySubject } from 'rxjs/ReplaySubject';
import { ConnectionMeta } from "app/classes/connnection-meta";

@Injectable()
export class ConnectionHistoryService {
  private readonly storageKey: string = 'connectionHistory';
  private connections: ConnectionMeta[] = [];
  private readonly connectionsSubject: ReplaySubject<ConnectionMeta[]> = new ReplaySubject(1);
  private readonly defaultConnection: ConnectionMeta = new ConnectionMeta('localhost', 'localhost');
  constructor(private localStorageService: LocalStorageService) {
    const connectionInStorage: string = localStorage.getItem(this.storageKey);
    if(connectionInStorage === null){
      //first time using this cookie
      this.store(this.defaultConnection);
      this.store(new ConnectionMeta('iPhone','192.168.0.15'));
      this.store(new ConnectionMeta('iPhone 5s','192.168.0.14'));
      this.store(new ConnectionMeta('iPhone 5','192.168.0.16'));
      this.store(new ConnectionMeta('iPhone 6','192.168.0.17'));
      this.store(new ConnectionMeta('Tims PC','192.168.0.81'));
      this.store(new ConnectionMeta('Google','8.8.8.8'));
    }else{
      this.connections = JSON.parse(connectionInStorage);
    }
    this.connectionsSubject.next(this.connections);
  }

  public get(): Observable<ConnectionMeta[]> {
    return this.connectionsSubject.asObservable();
  }

  public store(connection: ConnectionMeta) {
    //Only store if not already in history
    const alreadyStored = this.connections.some(stored => stored.address === connection.address);
    if (alreadyStored === false) {
      this.connections.push(connection);
      this.localStorageService.set(this.storageKey, JSON.stringify(this.connections));
      this.connectionsSubject.next(this.connections);
    }
  }

  public remove(name: string) {
    this.connections = this.connections.filter(connection => connection.name === name);
    this.localStorageService.set(this.storageKey, JSON.stringify(this.connections));
    this.connectionsSubject.next(this.connections);
  }
}
